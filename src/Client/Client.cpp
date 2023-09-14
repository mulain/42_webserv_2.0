#include "webserv.hpp"

Client::Client(const Config& config, std::vector<pollfd>& pollStructs, int fd, sockaddr_in address):
	_config(config),
	_pollStructs(pollStructs),
	_request(NULL),
	_response(NULL),
	_fd(fd), 
	_address(address),
	_append(false),
	_bytesWritten(0),
	_childBirth(0),
	_cgiInProgress(false)
{}

Client::Client(const Client& src):
	_config(src._config),
	_pollStructs(src._pollStructs)
{
	*this = src;
}

Client::~Client()
{
	if (_request)
		delete _request;
	
	if (_response)
		delete _response;
}

Client& Client::operator=(const Client& src)
{
	if (src._request != NULL)
		_request = new Request(*src._request);
	else
		_request = NULL;
	
	if (src._response != NULL)
		_response = src._response->clone();
	else
		_response = NULL;
	
	_fd = src._fd;
	_address = src._address;
	_buffer = src._buffer;
	_pollStruct = src._pollStruct;

	_append = src._append;
	_bytesWritten = src._bytesWritten;
	
	_cgiPid = src._cgiPid;
	_childBirth = src._childBirth;
	_cgiInProgress = src._cgiInProgress;
	
	_envVec = src._envVec;
	_env = src._env;
	_argvVec = src._argvVec;
	_argv = src._argv;

	return *this;
}

void Client::incomingData(std::vector<pollfd>::iterator pollStruct)
{
	_pollStruct = pollStruct;
	
	receive();
	
	if (!_request)
		newRequest();

	if (_request->internalScript())
		newResponse(_request->internalScript());
	else if (_request->method() == GET)
		handleGet();
	else if (_request->method() == POST)
		handlePost();
	else if (_request->method() == DELETE)
		handleDelete();
}

std::string Client::prependClassName(std::string function)
{
	return "Client::" + function;
}

void Client::receive()
{
	char	buffer[RECV_CHUNK_SIZE];

	int bytesReceived = recv(_fd, buffer, RECV_CHUNK_SIZE, 0);
	if (bytesReceived <= 0)
		throw CloseConnection(MYNAME, E_RECV);
	_buffer.append(buffer, bytesReceived);
}

void Client::newRequest()
{
	_request = new Request(_buffer, _config, *this);
	_request->process(); // has to be separate from constructor because this can throw
	_request->whoIsI();
}

int	Client::getFd() const { return _fd; }

const char* Client::getAddr() const { return inet_ntoa(_address.sin_addr); }

void Client::whoIsI() const
{
	std::stringstream title;
	title << "----- Client on fd " << _fd << " with session id " << _request->sessionID() << " -----";
	std::string separator(title.str().size(), '-');
	
	std::cout << "\n" << title.str() << "\n";
	_request->whoIsI();
	std::cout << separator << "\n" << std::endl;
}

void Client::newResponse(int code)
{
	if (_response)
		delete _response;
	
	std::string userPagePath = _request->statusPagePath(code);

	if (resourceExists(userPagePath))
		_response = new File(code, userPagePath, *_request);
	else
		_response = new DynContent(code, statusPage, *_request);
	_pollStruct->events = POLLOUT | POLLHUP;
}

void Client::newResponse(std::string sendPath)
{
	if (_response)
		delete _response;
	
	_response = new File(200, sendPath, *_request);
	_pollStruct->events = POLLOUT | POLLHUP;
}

void Client::newResponse(dynCont contentSelector)
{
	if (_response)
		delete _response;

	_response = new DynContent(200, contentSelector, *_request);
	_pollStruct->events = POLLOUT | POLLHUP;
}

void Client::sendStatusPage(int code)
{
	newResponse(code);
}

bool Client::outgoingData() // returns true if more to send
{
	if (_cgiInProgress)
	{
		if (handleCGI())
			return true; // CGI not done
	}
	return _response->send(_fd);
}

void Client::handleGet()
{
	if (_request->cgiRequest())
	{
		handleCGI();
		return;
	}

	if (isDirectory(_request->updatedURL()))
	{
		std::string stdFile = _request->updatedURL() + _request->standardFile();
		
		if (resourceExists(stdFile))
			newResponse(stdFile);
		else if (_request->dirListing())
			newResponse(dirListing);
		else
			newResponse(404);
	}
	else
	{
		if (resourceExists(_request->updatedURL()))
			newResponse(_request->updatedURL());
		else
			newResponse(404);
	}
}

void Client::handleDelete()
{
	if (isDirectory(_request->updatedURL())) // deleting directories not allowed
		newResponse(405);
	else if (!resourceExists(_request->updatedURL()))
		newResponse(404);
	else if (remove(_request->updatedURL().c_str()) == 0)
		newResponse(204);
	else
		newResponse(500);
}

void Client::handlePost()
{
	std::ofstream	outFile;
	std::string		path;
	
	if (_request->cgiRequest())
		path = _request->cgiIn();
	else
	{
		path = _request->updatedURL();
		
		if (resourceExists(path) && !_request->locationInfo()->delete_)
			throw ErrorCode(409, MYNAME); // if DELETE not allowed and file already exists
	}

	if (_append)
		outFile.open(path.c_str(), std::ios::binary | std::ios::app);
	else
	{
		outFile.open(path.c_str(), std::ios::binary | std::ios::trunc);
		_append = true;
	}

	if (!outFile)
	{
		outFile.close();
		throw ErrorCode(500, MYNAME);
	}

	outFile.write(_buffer.c_str(), _buffer.size());
	_bytesWritten += _buffer.size();
	_buffer.clear();
	outFile.close();

	if (_bytesWritten >= _request->contentLength())
	{
		if (_request->cgiRequest())
			handleCGI();
		else
			newResponse(201);
	}
}

bool Client::handleCGI()
{
	int status;
	
	if (!_cgiInProgress)
	{
		_pollStruct->events = POLLOUT | POLLHUP;
		launchChild();
	}
	
	if (waitpid(_cgiPid, &status, WNOHANG) == 0)
	{
		if (_childBirth + CGI_TIMEOUT < time(NULL))
		{
			std::cerr << E_CL_CHILDTIMEOUT << std::endl;
			kill(_cgiPid, SIGKILL);
			waitpid(_cgiPid, &status, 0); // have to wait for child to finish dying
		}
		else
			return true; // child still running, but not timed out.
	}
	
	_cgiInProgress = false;

	if (WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0) // WIFEXITED(status) == 0 -> child was interrupted
	{
		std::cerr << E_CL_CHILD << std::endl;
		throw ErrorCode(500, MYNAME);
	}
	
	if (resourceExists(_request->cgiOut()))
		newResponse(_request->cgiOut());
	else
		newResponse(500);

	if (_request->method() == POST && remove(_request->cgiIn().c_str()) != 0)
		std::cerr << E_CL_TEMPFILEREMOVAL << std::endl;
	
	return false;
}

void Client::launchChild()
{
	buildArgvEnv();

	if ((_cgiPid = fork()) == -1)
	{
		perror("fork");
		throw ErrorCode(500, MYNAME);
	}

	if (_cgiPid == 0)
	{
		for (size_t i = 0; i < _pollStructs.size(); ++i)
			close(_pollStructs[i].fd);
			
		execve(_request->cgiExecPath().c_str(), _argv.data(), _env.data());
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else
	{
		_childBirth = time(NULL);
		_cgiInProgress = true;
	}
}

void Client::buildArgvEnv()
{
	// argv
	_argvVec.push_back(_request->cgiExecPath()); // name of executable (giving path, but doesn't matter)
	_argvVec.push_back(_request->updatedURL()); // path to script

	for (size_t i = 0; i < _argvVec.size(); ++i)
		_argv.push_back(const_cast<char*>(_argvVec[i].c_str()));
	_argv.push_back(NULL);
	
	// env
	std::stringstream	contentLength, port;
	std::string			cookie, ipAddress, userAgent;
	
	contentLength << _request->contentLength();
	port << ntohs(_request->activeConfig()->getPort());

	if (_request->headers()->find("cookie") != _request->headers()->end())
		cookie = _request->headers()->find("cookie")->second;

	ipAddress = inet_ntoa(_address.sin_addr);

	if (_request->headers()->find("user-agent") != _request->headers()->end())
		userAgent = _request->headers()->find("user-agent")->second;
	
	_envVec.push_back("SCRIPT_NAME=" + _request->file());
	_envVec.push_back("QUERY_STRING=" + _request->queryString());
	_envVec.push_back("REQUEST_METHOD=" + _request->method());
	_envVec.push_back("CONTENT_TYPE=" + _request->contentType());
	_envVec.push_back("CONTENT_LENGTH=" + contentLength.str());
	_envVec.push_back("HTTP_COOKIE=" + cookie);
	_envVec.push_back("REMOTE_ADDR=" + ipAddress);
	_envVec.push_back("SERVER_NAME=" + _request->selectedHost());
	_envVec.push_back("SERVER_PORT=" + port.str());
	_envVec.push_back("PATH_INFO=" + _request->updatedURL());
	_envVec.push_back("HTTP_USER_AGENT=" + userAgent);
	_envVec.push_back("INPUT_FILE=" + _request->cgiIn());
	_envVec.push_back("OUTPUT_FILE=" + _request->cgiOut());

	for (size_t i = 0; i < _envVec.size(); ++i)
		_env.push_back(const_cast<char*>(_envVec[i].c_str()));
	_env.push_back(NULL);
}

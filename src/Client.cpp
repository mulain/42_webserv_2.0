#include "../include/Client.hpp"

Client::Client(const Config& config, pollfd& pollStruct, sockaddr_in address):
	_config(config),
	_pollStruct(pollStruct),
	_request(NULL),
	_response(NULL),
	_fd(pollStruct.fd), 
	_address(address),
	_append(false),
	_bytesWritten(0),
	_childLaunched(false)
{}

Client::Client(const Client& src):
	_config(src._config),
	_pollStruct(src._pollStruct)
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
	std::cout << "Client = operator" <<std::endl;
	
	if (src._request != NULL)
		_request = new Request(*src._request);
	else
		_request = NULL;
	if (src._response != NULL)
		_response = _response->clone();
	else
		_response = NULL;
	_fd = src._fd;
	_address = src._address;
	_buffer = src._buffer;
	_append = src._append;
	_bytesWritten = src._bytesWritten;
	_childLaunched = src._childLaunched;
	_childBirth = src._childBirth;
	_cgiExecPath = src._cgiExecPath;
	_envVec = src._envVec;
	_env = src._env;
	_argvVec = src._argvVec;
	_argv = src._argv;
	_cgiPid = src._cgiPid;
	_parentToChild[0] = src._parentToChild[0];
	_parentToChild[1] = src._parentToChild[1];
	_childToParent[0] = src._childToParent[0];
	_childToParent[1] = src._childToParent[1];
	return *this;
}

void Client::incomingData()
{
	receive();
	if (!_request)
		newRequest();
	// maybe handle cgi partition here
	if (_request->method() == GET)
	{
		if (_request->cgiRequest())
			handleGetCGI(); // shmangidy
		else
			handleGet();
	}
	else if (_request->method() == POST)
	{
		if (_request->cgiRequest())
			handlePostCGI(); // shmangidy
		else
			handlePost();
	}
	else if (_request->method() == DELETE)
		handleDelete();
}

void Client::receive()
{
	char buffer[RECV_CHUNK_SIZE];
	int bytesReceived = recv(_fd, buffer, RECV_CHUNK_SIZE, 0);
	if (bytesReceived <= 0)
		throw NetworkFailure(__FUNCTION__);
	_buffer.append(buffer, bytesReceived);
}

void Client::newRequest()
{
	ANNOUNCEME
	_request = new Request(_buffer, _config, *this);
	_request->process();
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

// gets prioritized over template overload
void Client::newResponse(int code)
{
	ANNOUNCEME
	if (_response)
		delete _response;
	_pollStruct.events = POLLOUT | POLLHUP;

	std::string userPagePath = _request->statusPagePath(code);

	if (resourceExists(userPagePath))
		_response = new Response(userPagePath, *_request);
	else
		_response = new StatusPage(code, *_request);
}

template<typename Arg>
void Client::newResponse(Arg arg)
{
	ANNOUNCEME
	if (_response)
		delete _response;
	_pollStruct.events = POLLOUT | POLLHUP;
	_response = new Response(arg, *_request);
}

void Client::sendStatusPage(int code)
{
	newResponse(code);
}

bool Client::outgoingData()
{
	ANNOUNCEME
	if (!_response)
		std::cout << __FUNCTION__ << MMMMMEGAERROR << std::endl;
	
	return (_response->send(_fd));
}

void Client::handleGet()
{
	ANNOUNCEME
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
	std::ofstream	outputFile;
	
	if (_append)
		outputFile.open(_request->updatedURL().c_str(), std::ios::binary | std::ios::app);
	else
	{
		outputFile.open(_request->updatedURL().c_str(), std::ios::binary | std::ios::trunc);
		_append = true;
	}
	if (!outputFile)
	{
		outputFile.close();
		throw ErrorCode(500, __FUNCTION__);
	}
	outputFile.write(_buffer.c_str(), _buffer.size());
	_bytesWritten += _buffer.size();
	_buffer.clear();
	outputFile.close();

/* 	if (_bytesWritten >= _contentLength)
		sendEmptyStatus(201); */
}

void Client::handleGetCGI()
{

}

void Client::handlePostCGI()
{
	
}

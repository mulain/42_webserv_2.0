#include "../include/Server.hpp"

Server::Server(const Config& config)
{	
	_server_fd = -1;
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.s_addr = config.getHost();
	_serverAddress.sin_port = config.getPort();

	_configs = config.getAltConfigs();
	_configs.insert(_configs.begin(), config);
	applyHostConfig(config);	
}

void Server::applyHostConfig(const Config& config)
{
	_names = config.getNames();
	_root = config.getRoot();
	_defaultDirListing = config.getDefaultDirlisting();
	_clientMaxBody = config.getClientMaxBody();
	_maxConnections = config.getMaxConnections();
	_standardFile = config.getStandardFile();
	_statusPagePaths = config.getStatusPagePaths();
	_locations = config.getLocations();
	_cgiPaths = config.getCgiPaths();
	_mimeTypes = config.getMIMETypes();
}

Server::~Server()
{
	if (_server_fd != -1)
		std::cout << "Server destructor on listening fd " << _server_fd << "." << std::endl;
}

const Config& Server::getConfig()
{
	return _configs[0];
}

int Server::fd()
{
	return _server_fd;
}

void Server::whoIsI()
{
	std::cout	<< '\n';
				if (!_names.empty())
				{
					std::cout << "Name(s):\t" << *_names.begin() << '\n';
					for (strVec_it it = ++_names.begin(); it != _names.end(); ++it)
						std::cout << "\t\t" << *it << '\n';
				}
	std::cout	<< "Host:\t\t" << inet_ntoa(_serverAddress.sin_addr) << '\n'
				<< "Port:\t\t" << ntohs(_serverAddress.sin_port) << '\n'
				<< "Root:\t\t" << _root << '\n'
				<< "Dflt. dir_list:\t" << (_defaultDirListing ? "yes" : "no") << '\n'
				<< "Cl. max body:\t" << _clientMaxBody << '\n'
				<< "Max Conns:\t" << _maxConnections << '\n'
				<< "standardfile:\t" << _standardFile << '\n';
				if (!_statusPagePaths.empty())
				{
					std::cout << "Error Pages:\t" << _statusPagePaths.begin()->first << '\t' << _statusPagePaths.begin()->second << '\n';
					for (intStrMap_it it = ++_statusPagePaths.begin(); it != _statusPagePaths.end(); it++)
						std::cout << "\t\t" << it->first << '\t' << it->second << std::endl;
				}
				if (!_locations.empty())
				{
					std::cout	<< "Known loctns:\t" << _locations.begin()->first << '\n';
					for (strLocMap_it it = ++_locations.begin(); it != _locations.end(); it++)
						std::cout << "\t\t" << it->first << '\n';
				}
				if (!_cgiPaths.empty())
				{
					std::cout	<< "CGI Paths:\t" << _cgiPaths.begin()->first << '\t' << _cgiPaths.begin()->second << '\n';
					for (strMap_it it = ++_cgiPaths.begin(); it != _cgiPaths.end(); it++)
						std::cout << "\t\t" << it->first << '\t' << it->second << std::endl;
				}
}

void Server::startListening(std::vector<pollfd>& pollVector)
{
	int		options = 1;
	
	_server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd == -1)
		closeFdAndThrow(_server_fd);
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&options, sizeof(options)) == -1)
		closeFdAndThrow(_server_fd);;
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
		closeFdAndThrow(_server_fd);
	if (bind(_server_fd, (struct sockaddr*) &_serverAddress, sizeof(_serverAddress)) == -1)
		closeFdAndThrow(_server_fd);
	if (listen(_server_fd, SOMAXCONN) == -1)
		closeFdAndThrow(_server_fd);
	
	_pollVector = &pollVector;
	addPollStruct(_server_fd, POLLIN);
}

/* 

bool Server::receive()
{
	if (!(_pollStruct->revents & POLLIN))
		return false;
	char buffer[RECV_CHUNK_SIZE];
	int bytesReceived = recv(_clientIt->fd, buffer, RECV_CHUNK_SIZE, 0);
	if (bytesReceived <= 0)
	{
		closeClient("Connection closed (no data received).");
		return false;
	}
	_clientIt->buffer.append(buffer, bytesReceived);
	_clientIt->bufferCopy = buffer;
	return true;
} */

/* 




void Server::cgiPost_launchChild()
{
	buildCGIvars();

	// Need two pipes because we are sending and receiving in both child and parent.
	// Parent would read its own sent data before the child has a chance to if using only one pipe
	if (pipe(_clientIt->parentToChild) == -1 || pipe(_clientIt->childToParent) == -1)
	{
		std::cerr << E_PIPE << std::endl;
		sendStatusPage(500);
		return;
	}

	_clientIt->cgiPid = fork();
	if (_clientIt->cgiPid == -1)
	{
		std::cerr << E_FORK << std::endl;
		sendStatusPage(500);
		return;
	}
	if (_clientIt->cgiPid == 0)
	{
		// close socket fds that the child inherited 
		for (std::vector<pollfd>::iterator it = _pollVector->begin(); it != _pollVector->end(); ++it)
			close(it->fd);

		if (dup2(_clientIt->parentToChild[0], STDIN_FILENO) == -1 // move stdin to read end of pipe on which the parent will send
			|| dup2(_clientIt->childToParent[1], STDOUT_FILENO) == -1) // move stdout to write end of pipe on which the child will send
		{
			std::cerr << E_DUP2 << std::endl;
			exit(EXIT_FAILURE);
		}
		close(_clientIt->parentToChild[1]); // the parent will write here, the child doesn't need it
		close(_clientIt->childToParent[0]); // the parent will read here, the child doesn't need it

		execve(_cgiExecPath.c_str(), _clientIt->argv.data(), _clientIt->env.data());
		std::cerr << E_EXECVE << std::endl;
		close(_clientIt->parentToChild[0]);
		close(_clientIt->childToParent[1]);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(_clientIt->parentToChild[0]);
		close(_clientIt->childToParent[1]);
		_clientIt->childLaunched = true;
		// not waiting for child because we might have to go through another loop to finish sending
	}
}

// prolly also test for bigger files. Only tested with single loop completion
void Server::recvResponseBody_CGI()
{
	if (!_clientIt->childLaunched)
		cgiPost_launchChild();
	int bytesWritten = write(_clientIt->parentToChild[1], _clientIt->buffer.c_str(), _clientIt->buffer.size());
	if (bytesWritten == -1)
	{
		kill(_clientIt->cgiPid, SIGKILL);
		close(_clientIt->parentToChild[1]);
		close(_clientIt->childToParent[0]);
		closeClient(E_WRITE_CHPIPE);
		throw std::runtime_error(__FUNCTION__);
	}
	_clientIt->bytesWritten += bytesWritten;
	_clientIt->buffer.erase(0, bytesWritten);
	if (_clientIt->bytesWritten >= _clientIt->contentLength)
	{
		close(_clientIt->parentToChild[1]);
		
		if (childFinished())
			sendFile_200("");
		else
			sendStatusPage(500);
	}
}

bool Server::childFinished()
{
	int status;
	
	int childDone = waitpid(_clientIt->cgiPid, &status, WNOHANG);
	if (childDone == 0) // child still running
	{
		if (_clientIt->childBirth + 10 < time(NULL))
		{
			std::cerr << E_CHILDTIMEOUT << std::endl;
			close(_clientIt->childToParent[0]);
			removePollStruct(_clientIt->childToParent[0]);
			sendStatusPage(500);
			throw std::runtime_error(__FUNCTION__);
		}
		return false;

	}
	if (childDone == -1 || WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0) // WIFEXITED(status) returns 0 if child did not exit normally
	{
		std::cerr << E_CHILD << std::endl;
		close(_clientIt->childToParent[0]);
		removePollStruct(_clientIt->childToParent[0]);
		sendStatusPage(500);
		throw std::runtime_error(__FUNCTION__);
	}
	return true;
}

void Server::handlePost()
{
	if (_clientIt->state > handleRequest)
		return;
	
	if (_clientIt->state == recv_reqBody)
		receive();
	
	if (_clientIt->cgiRequest)
	{
		recvResponseBody_CGI();
		return;
	}
	
	if (!resourceExists(_clientIt->updatedDirectory))
	{
		sendStatusPage(500);
		return;
	}
	std::ofstream outputFile;
	if (_clientIt->append)
		outputFile.open(_clientIt->updatedURL.c_str(), std::ios::binary | std::ios::app);
	else
	{
		outputFile.open(_clientIt->updatedURL.c_str(), std::ios::binary | std::ios::trunc);
		_clientIt->append = true;
	}
	if (!outputFile)
	{
		std::cerr << E_POSTFILE << std::endl;
		sendStatusPage(500);
		return;		
	}
	outputFile.write(_clientIt->buffer.c_str(), _clientIt->buffer.size());
	_clientIt->bytesWritten += _clientIt->buffer.size();
	_clientIt->buffer.clear();
	outputFile.close();

	if (_clientIt->bytesWritten >= _clientIt->contentLength)
		sendEmptyStatus(201);
}


 */




/* void Server::cgiSendError(const char* msg)
{
	kill(_clientIt->cgiPid, SIGKILL);
	close(_clientIt->childToParent[0]);

	closeClient(msg);
	throw std::runtime_error(__FUNCTION__);
} */

/* void Server::sendResponseBody_CGI()
{
	if (!(getPollStruct(_clientIt->childToParent[0])->revents & POLLIN))
		return;

	char buffer[SEND_CHUNK_SIZE];
	
	int bytesRead = read(_clientIt->childToParent[0], buffer, SEND_CHUNK_SIZE);
	if (bytesRead == -1)
		cgiSendError(E_READ);
	if (bytesRead != 0)
	{
		// chunk encoding needs the size of the data first
		std::stringstream message;
		message << std::hex << bytesRead; 
		message << "\r\n";
		message.write(buffer, bytesRead);
		message << "\r\n";

		if (send(_clientIt->fd, message.str().c_str(), message.str().size(), 0) <= 0)
			cgiSendError(E_SEND);
	}
	else
	{
		if (send(_clientIt->fd, "0\r\n\r\n", 5, 0) <= 0) // send chunk transfer end chunk
			cgiSendError(E_SEND);
		close(_clientIt->childToParent[0]);
		removePollStruct(_clientIt->childToParent[0]);
		closeClient("Sending complete (chunked transfer from pipe).");
		return;
	}
} */



/*
A better way to handle this would be to not write the data from the ServerConfig to
the Server object (applyConfig()), but to just point to the currently active ServerConfig and call
its getters.
This requires a bigger refactor, because some stuff would have to be tested before the server constructor
(in ConfigFile / ServerConfig). Doable, but a bit of work. Maybe some day.
*/
/* void Server::selectHostConfig()
{
	if (_clientIt->host.empty())
	{
		applyHostConfig(_configs[0]); // first config is the default one
		_activeServerName = _configs[0].getNames()[0];
		return;
	}
	for (size_t i = 0; i < _configs.size(); ++i)
	{
		if (stringInVec(_clientIt->host, _configs[i].getNames()))
		{
			std::cout << "Hostname '" << _clientIt->host << "' found in ServerConfig #" << i << std::endl;
			applyHostConfig(_configs[i]);
			_activeServerName = _clientIt->host;
			return;
		}
	}
	std::cout << "Hostname '" << _clientIt->host << "' not found. Running default ServerConfig." << std::endl;
	applyHostConfig(_configs[0]);
	_activeServerName = _configs[0].getNames()[0];
} */

/* bool Server::requestError()
{
	// wrong protocol
	if (_clientIt->httpProtocol != HTTPVERSION)
		return (sendStatusPage(505), true);
	
	// method not supported by server
	if (_clientIt->method != GET
		&& _clientIt->method != POST
		&& _clientIt->method != DELETE)
		return (sendStatusPage(501), true);

	// invalid URL for our purposes; but also so we don't have to always guard later when looking for "/"
	if (_clientIt->URL.find("/") == std::string::npos)
		return (sendStatusPage(404), true);

	// body size too large
	if (_clientIt->contentLength > _clientMaxBody)
		return (sendStatusPage(413), true); 
	
	// check requested resource
	strLocMap_it locIt = _locations.find(_clientIt->directory);
		
	// access forbidden (have to specifically allow each path in config file)
	if (locIt == _locations.end())
		return (sendStatusPage(404), true); // returning 404, not 403, to not leak file structure

	// access granted, but not for the requested method
	if ((_clientIt->method == GET && !locIt->second.get)
		|| (_clientIt->method == POST && !locIt->second.post)
		|| (_clientIt->method == DELETE && !locIt->second.delete_))
		return (sendStatusPage(405), true);
	
	return false;
}
 */
/* void Server::removePollStruct(int fd)
{
	std::vector<pollfd>::iterator it = _pollVector->begin();
	
	while (it != _pollVector->end() && it->fd != fd)
		++it;
	if (it == _pollVector->end())
		throw std::runtime_error(__FUNCTION__);
	_pollVector->erase(it);
}
 */
/* void Server::closeClient(const char* msg)
{
	if (msg)
		std::cout << "closeClient on fd " << _clientIt->fd << ": " << msg << std::endl;
	close(_clientIt->fd);

	removePollStruct(_clientIt->fd);

	

	// erase client and decrement _index to not skip the next client in the for loop
	_clients.erase(_clientIt);
	--_index;
} */

pollfd* Server::getPollStruct(int fd)
{
	std::vector<pollfd>::iterator it = _pollVector->begin();
		
	while (it != _pollVector->end() && it->fd != fd)
		++it;
	if (it == _pollVector->end())
		throw std::runtime_error(__FUNCTION__);
	return &*it;
}

std::string	Server::mimeType(const std::string& filepath)
{
	std::string extension;
	strMap_it	it;
	std::string defaultType = "application/octet-stream";

	extension = fileExtension(filepath);
	it = _mimeTypes->find(extension);
	if (it != _mimeTypes->end())
		return it->second;
	return defaultType;
}


/* 
void Server::buildCGIvars()
{
	// argv
	
	// allocate for argv
	_clientIt->argvVec.push_back(_cgiExecPath.c_str()); // name of executable (giving path here but hey)
	_clientIt->argvVec.push_back(_clientIt->updatedURL.c_str()); // path to script, which is the requested file
	
	// store pointers to allocated strings in 2d-char compatible format
	for (size_t i = 0; i < _clientIt->argvVec.size(); ++i)
		_clientIt->argv.push_back(const_cast<char*>(_clientIt->argvVec[i].c_str()));
	_clientIt->argv.push_back(NULL);

	// env
	
	// prepare non insta-insertables
	std::stringstream contentLength;
	contentLength << _clientIt->contentLength; // best way I found to convert numerical to str in cpp98

	std::string cookie;
	if (_clientIt->headers.find("cookie") != _clientIt->headers.end())
		cookie = _clientIt->headers["cookie"];

	std::string ipAddress = inet_ntoa(_clientIt->address.sin_addr);

	std::stringstream port;
	port << ntohs(_serverAddress.sin_port);

	std::string userAgent;
	if (_clientIt->headers.find("user-agent") != _clientIt->headers.end())
		userAgent = _clientIt->headers["user-agent"];

	// allocate for env
	_clientIt->envVec.push_back("SCRIPT_NAME=" + _clientIt->filename);
	_clientIt->envVec.push_back("QUERY_STRING=" + _clientIt->queryString);
	_clientIt->envVec.push_back("REQUEST_METHOD=" + _clientIt->method);
	_clientIt->envVec.push_back("CONTENT_TYPE=" + _clientIt->contentType);
	_clientIt->envVec.push_back("CONTENT_LENGTH=" + contentLength.str());
	_clientIt->envVec.push_back("HTTP_COOKIE=" + cookie);
	_clientIt->envVec.push_back("REMOTE_ADDR=" + ipAddress);
	_clientIt->envVec.push_back("SERVER_NAME=" + _activeServerName);
	_clientIt->envVec.push_back("SERVER_PORT=" + port.str());
	_clientIt->envVec.push_back("PATH_INFO=" + _clientIt->updatedURL);
	_clientIt->envVec.push_back("HTTP_USER_AGENT=" + userAgent);
	
	// store pointers to allocated strings in 2d-char compatible format
	for (size_t i = 0; i < _clientIt->envVec.size(); ++i)
		_clientIt->env.push_back(const_cast<char*>(_clientIt->envVec[i].c_str()));
	_clientIt->env.push_back(NULL);
}
 */
void Server::addPollStruct(int fd, short flags)
{
	pollfd new_pollStruct;
	new_pollStruct.fd = fd;
	new_pollStruct.events = flags;
	new_pollStruct.revents = 0;
	_pollVector->push_back(new_pollStruct);
}
/* 
void Server::cgiChildGET()
{
	buildCGIvars();

	if (pipe(_clientIt->childToParent) == -1)
	{
		std::cerr << E_PIPE << std::endl;
		sendStatusPage(500);
		return;
	}
	if (fcntl(_clientIt->childToParent[0], F_SETFL, O_NONBLOCK) == -1
		|| fcntl(_clientIt->childToParent[1], F_SETFL, O_NONBLOCK) == -1)
	{
		close(_clientIt->childToParent[0]);
		close(_clientIt->childToParent[1]);
		std::cerr << E_FCNTL << std::endl;
		sendStatusPage(500);
		return;
	}
	
	addPollStruct(_clientIt->childToParent[0], POLLIN);

	_clientIt->cgiPid = fork();
	if (_clientIt->cgiPid == -1)
	{
		std::cerr << E_FORK << std::endl;
		sendStatusPage(500);
		return;
	}
	if (_clientIt->cgiPid == 0) // for explanations see cgiChildPOST()
	{
		for (std::vector<pollfd>::iterator it = _pollVector->begin(); it != _pollVector->end(); ++it)
			close(it->fd);
		//close(_clientIt->childToParent[0]); its already in the pollvector
		
		if (dup2(_clientIt->childToParent[1], STDOUT_FILENO) == -1)
		{
			std::cerr << E_DUP2 << std::endl;
			exit(EXIT_FAILURE);
		}
		
		execve(_cgiExecPath.c_str(), _clientIt->argv.data(), _clientIt->env.data());
		std::cerr << E_EXECVE << std::endl;
		close(_clientIt->childToParent[1]);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(_clientIt->childToParent[1]);
		sendFile_200("");
		_clientIt->childBirth = time(NULL);
	}
}
 */

#include "../include/Client.hpp"

void Client::requestHead()
{
	if (_state > recv_reqHead) // done receiving request head CHANGE LATER
		return;

	if (_buffer.find("\r\n") == std::string::npos)
	{
		sendStatusPage(400);
		return;
	}
	parseRequestLine();
	if (_buffer.find("\r\n\r\n") == std::string::npos)
	{
		if (_buffer.size() >= MAX_HEADERSIZE) //This only makes sense if the readchunk is big enough.
			sendStatusPage(431);
		else
			sendStatusPage(400); // we read the entire "headers" but they weren't properly terminated.
		return;
	}
	parseRequestHeaders();

	
	handleSession();
	selectServerConfig();
	updateVars();
}

void Client::parseRequestLine()
{
	_method = splitEraseStr(_buffer, " ");
	_URL = splitEraseStr(_buffer, " ");
	_httpProtocol = splitEraseStr(_buffer, "\r\n");

	size_t questionMarkPos = _URL.find("?");
	if (questionMarkPos != std::string::npos)
	{
		_queryString = _URL.substr(questionMarkPos + 1);
		_URL = _URL.substr(0, questionMarkPos);
	}
}

void Client::parseRequestHeaders()
{
	_headers = parseStrMap(_buffer, ":", "\r\n", "\r\n");
	
	if (_headers.find("host") != _headers.end())
		_host = _headers["host"];
	
	if (_headers.find("content-length") != _headers.end())
		_contentLength = atoi(_headers["content-length"].c_str());
	
	if (_headers.find("content-type") != _headers.end())
		_contentType = _headers["content-type"];

	if (_headers.find("cookie") != _headers.end())
	{
		std::string temp = _headers["cookie"]; // parseStrMap erases from the input string. We want to preserve the cookie header to be able to pass it to CGI
		_cookies = parseStrMap(temp, "=", ";", "Please parse me to the end!");
	}
}



void Client::selectServerConfig()
{
	if (_host.empty() || stringInVec(_host, _config.getNames()))
	{
		_activeConfig = &_config;
		return;
	}
	for (size_t i = 0; i < _config.getAltConfigs().size(); ++i)
	{
		if (stringInVec(_host, _config.getAltConfigs()[i].getNames()))
		{
			_activeConfig = &_config.getAltConfigs()[i];
			return;
		}
	}
	_activeConfig = &_config;
	std::cout << "Hostname '" << _host << "' not found. Running default ServerConfig." << std::endl;
}

void Client::updateVars()
{
	_URL = ifDirAppendSlash(_URL);
	_directory = _URL.substr(0, _URL.find_last_of("/") + 1);
	_filename = _URL.substr(_URL.find_last_of("/") + 1);
	
	_dirListing = dirListing(_directory);
	
	strLocMap_it loc = _activeConfig->getLocations().find(_directory);
	if (loc == _activeConfig->getLocations().end())
		return; // this means request denied, but will not be dealt with here.
	
	_standardFile =  loc->second.std_file;
	if (_standardFile.empty())
		_standardFile = _activeConfig->getStandardFile();
	
	std::string	http_redir = loc->second.http_redir;
	if (!http_redir.empty())
		_updatedDirectory = http_redir;
	else if (_method == POST && !loc->second.upload_dir.empty())
		_updatedDirectory = loc->second.upload_dir;
	else
		_updatedDirectory = _directory;

	_updatedDirectory = prependRoot(_updatedDirectory); // need to preserve old dirs for dirlisting
	_updatedURL = _updatedDirectory + _filename;
}

bool Client::dirListing(const std::string& directory)
{
	strLocMap_it loc = _activeConfig->getLocations().find(directory);
	
	if (loc == _activeConfig->getLocations().end())
		return false;
	if (loc->second.dir_listing == "yes")
		return true;
	if (loc->second.dir_listing == "no")
		return false;
	if (!_activeConfig->getDefaultDirlisting())
		return false;
	return true;
}

std::string Client::buildResponseHead()
{
	std::stringstream ss_header;
	size_t contentLength = fileSize(_sendPath);
	
	ss_header		<< HTTPVERSION << ' ' << _statusCode << ' ' << getHttpMsg(_statusCode) << "\r\n"
					<< "Server: " << SERVERVERSION << "\r\n"
					<< "connection: close" << "\r\n";
	if (_cgiRequest)
		ss_header	<< "transfer-encoding: chunked\r\n"
					<< "content-type: " << mimeType(".html") << "\r\n"; // we only return html when it is a CGI request 
	else
	{
		ss_header << "content-length: " << contentLength << "\r\n";
		if (contentLength != 0)
			ss_header << "content-type: " << mimeType(_sendPath) << "\r\n";
	}
	if (_setCookie)
		ss_header << cookie(SESSIONID, _sessionId, 3600, "/") << "\r\n";
	ss_header << "\r\n";
	return ss_header.str();
}

 
std::string Client::cookie(const std::string& key, const std::string& value, int expiration, const std::string& path)
{
	std::stringstream cookie;
	cookie << "set-cookie: " << key << "=" << value << ";";
	if (expiration >= 0)
		cookie << "max-age=" << expiration << ";";
	cookie << "path=" << path << ";";
	return cookie.str();
}

void Client::trackSession()
{
	/* if (_request->cookies().find(SESSIONID) != _request->cookies().end())
		_sessionId = _request->cookies()[SESSIONID];
	else
	{
		_sessionId = generateSessionId();
		_setCookie = true;
	} */

	std::string 	logPath = SYS_LOGS + _sessionId + ".log";
	std::ofstream	logFile(logPath.c_str(), std::ios::app);
	
	if (logFile)
	{
		logFile << currentTime() << "\t" << inet_ntoa(_address.sin_addr) << "\t" << _request->method() << " " << _request->URL() << "\n";
		logFile.close();
	}
	else
	{
		perror(__FUNCTION__);
		logFile.close();
	}
	generateSessionLogPage(logPath); // handle this dynamically
}

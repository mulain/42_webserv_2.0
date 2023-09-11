#include "webserv.hpp"

Request::Request(std::string& bufferRef, const Config& config, const Client& client):
	buffer(&bufferRef), // non const buffer from Client
	_config(&config),
	_activeConfig(&config),
	_client(&client),
	_cgiRequest(false),
	_internalScript(no)
{}

Request::Request(const Request& src)
{
	*this = src;
}

Request& Request::operator=(const Request& src)
{
	buffer = src.buffer;
	_config = src._config;
	_activeConfig = src._activeConfig;
	_client = src._client;
	_method = src._method;
	_URL = src._URL;
	_httpProtocol = src._httpProtocol;
	_queryString = src._queryString;
	_headers = src._headers;
	_cookies = src._cookies;
	_host = src._host;
	_contentLength = src._contentLength;
	_contentType = src._contentType;
	_sessionID = src._sessionID;
	_directory = src._directory;
	_file = src._file;
	_cgiRequest = src._cgiRequest;
	_internalScript = src._internalScript;
	_setCookie = src._setCookie;
	_standardFile = src._standardFile;
	_updatedDirectory = src._updatedDirectory;
	_updatedURL = src._updatedURL;
	_cgiExecPath = src._cgiExecPath;
	
	return *this;
}


void Request::process()
{
	parseRequestLine();
	parseRequestHeaders();
	handleSessionID();
	trackSession();
	selectConfig();
	requestError();
	updateVars();
}

void Request::parseRequestLine()
{
	if (buffer->find("\r\n") == std::string::npos)
		throw ErrorCode(400, __FUNCTION__);
	
	_method = splitEraseStr(*buffer, " ");
	_URL = splitEraseStr(*buffer, " ");
	_httpProtocol = splitEraseStr(*buffer, "\r\n");

	_URL = appendSlash(_URL);
	_directory = _URL.substr(0, _URL.find_last_of("/") + 1);
	_file = _URL.substr(_URL.find_last_of("/") + 1);

	size_t questionMarkPos = _URL.find("?");
	
	if (questionMarkPos != std::string::npos)
	{
		_queryString = _URL.substr(questionMarkPos + 1);
		_URL = _URL.substr(0, questionMarkPos);
	}
}

void Request::parseRequestHeaders()
{
	if (buffer->find("\r\n\r\n") == std::string::npos)
	{
		if (buffer->size() >= MAX_HEADERSIZE) // readchunk must be bigger than size of requestLine + maxheadersize
			throw ErrorCode(431, __FUNCTION__);
		else
			throw ErrorCode(400, __FUNCTION__);
	}
	_headers = parseStrMap(*buffer, ":", "\r\n", "\r\n");
	
	if (_headers.find("host") != _headers.end())
		_host = _headers["host"];
	
	if (_headers.find("content-length") != _headers.end())
	{
		std::istringstream iss(_headers["content-length"]);
		iss >> _contentLength;
	}
	else
		_contentLength = 0;
	
	if (_headers.find("content-type") != _headers.end())
		_contentType = _headers["content-type"];

	if (_headers.find("cookie") != _headers.end())
	{
		std::string temp = _headers["cookie"]; // parseStrMap erases from the input string. We want to preserve the cookie header to be able to pass it to CGI
		_cookies = parseStrMap(temp, "=", ";", "");
	}
}

void Request::handleSessionID()
{
	if (_cookies.find(SESSIONID) != _cookies.end() && !_cookies.find(SESSIONID)->second.empty())
	{
		_sessionID = _cookies[SESSIONID];
		_setCookie = false;
	}
	else
	{
		_sessionID = generateSessionId();
		_setCookie = true;
	}
}

void Request::trackSession()
{
	std::string 	logPath = SYS_LOGS + _sessionID + ".log";
	std::ofstream	logFile(logPath.c_str(), std::ios::app);
	
	if (logFile)
	{
		logFile << currentTimeCPP98() << "\t" << _client->getAddr() << "\t" << _method << " " << _URL << "\n";
		logFile.close();
	}
	else
	{
		perror(__FUNCTION__);
		logFile.close();
	}
}

void Request::selectConfig()
{
	if (_host.empty() || isStringInVec(_host, _config->getNames()))
	{
		std::cout << "host '" << _host << "' belongs to default config." << std::endl;
		_activeConfig = _config;
		return;
	}
	for (size_t i = 0; i < _config->getAltConfigs().size(); ++i)
	{
		if (isStringInVec(_host, _config->getAltConfigs()[i].getNames()))
		{
			std::cout << "host '" << _host << "' belongs to alternative config #" << i << "." << std::endl;
			_activeConfig = &_config->getAltConfigs()[i];
			return;
		}
	}
	_activeConfig = _config;
}

void Request::requestError()
{
	if (_httpProtocol != HTTPVERSION)
		throw ErrorCode(505, __FUNCTION__);

	if (_method != GET && _method != POST && _method != DELETE)
		throw ErrorCode(501, __FUNCTION__);
		
	if (_URL.find("/") == std::string::npos)
		throw ErrorCode(404, __FUNCTION__);
	
	if (!resourceExists(prependRoot(_URL)))
		throw ErrorCode(404, __FUNCTION__);

	if (_contentLength > _activeConfig->getClientMaxBody())
		throw ErrorCode(413, __FUNCTION__);

	_locInfo = _activeConfig->getLocations()->find(_directory);
	if (_locInfo == _activeConfig->getLocations()->end())
		throw ErrorCode(403, __FUNCTION__); // should return 404 on a production system to not leak file structure
		
	else if ((_method == GET && !_locInfo->second.get)
		|| (_method == POST && !_locInfo->second.post)
		|| (_method == DELETE && !_locInfo->second.delete_))
		throw ErrorCode(405, __FUNCTION__);
}

void Request::updateVars()
{	
	std::string extension = fileExtension(_file);

	if (extension == ".shmang")
	{
		if (_file == "sessionLog.shmang")
			_internalScript = sessionLog;
		/*
		else if (_file == "anotherDynamicContent.shmang")
			_internalScript = anotherDynCont;
		...
		*/
		
	}
	else if (_activeConfig->getCgiPaths()->find(extension) != _activeConfig->getCgiPaths()->end())
	{
		_cgiExecPath = _activeConfig->getCgiPaths()->find(extension)->second;
		_cgiRequest = true;
	}
	
	_standardFile = _locInfo->second.std_file;
	if (_standardFile.empty())
		_standardFile = _activeConfig->getStandardFile();
	
	/* // old version
	std::string	http_redir = loc->second.http_redir;
	if (!http_redir.empty())
		_updatedDirectory = http_redir;
	else if (_method == POST && !loc->second.upload_dir.empty())
		_updatedDirectory = loc->second.upload_dir;
	else
		_updatedDirectory = _directory;

	*/

	std::string	http_redir = _locInfo->second.http_redir;
	if (_method == POST && !_locInfo->second.upload_dir.empty()) // upload_redir supercedes http_redir
	{
		if (!resourceExists(_locInfo->second.upload_dir))
			throw ErrorCode(500, __FUNCTION__);
		_updatedDirectory = _locInfo->second.upload_dir;
	}
	else if (!http_redir.empty())
		_updatedDirectory = http_redir;
	else
		_updatedDirectory = _directory;

	_updatedDirectory = prependRoot(_updatedDirectory);
	_updatedURL = _updatedDirectory + _file;
}

std::string Request::prependRoot(const std::string& path) const
{
	if (path.find('/') == 0)
		return _activeConfig->getRoot() + path.substr(1);
	else
		return path;
}

std::string Request::appendSlash(const std::string& path)
{
	std::string newPath = path;
	
	if (isDirectory(prependRoot(newPath)) && newPath[newPath.size() - 1] != '/')
		newPath.append("/");
	return newPath;
}

void Request::whoIsI() const
{
	std::stringstream ss;
	ss	<< "----- Request from Client on fd " << _client->getFd()
		<< " with session id: " << _sessionID << " -----";

	std::string separator(ss.str().size(), '-');

	std::cout	<< "\n" << ss.str() << "\n"
				<< "host:\t\t" << _host << "\n"
				<< "active config:\t" << (_activeConfig == _config ? "default config" : "alt config") << "\n"
				<< "method:\t\t" << _method << "\n"
				<< "URL:\t\t" << _URL << "\n"
				<< "dir:\t\t" << _directory << "\n"
				<< "file:\t\t" << _file << "\n"
				<< "content-length:\t" << _contentLength << "\n"
				<< "content-type:\t" << _contentType << "\n"
				<< "standardfile:\t" << _standardFile << "\n"
				<< "dirlisting:\t" << (dirListing() ? "yes" : "no") << "\n"
				<< "updatedURL:\t" << _updatedURL << "\n"
				<< "updatedDir:\t" << _updatedDirectory << "\n"
				<< "query string:\t" << _queryString << "\n"
				<< separator << std::endl;
}

const Config*	Request::activeConfig() const { return _activeConfig; }

std::string Request::method() const { return _method; }

std::string Request::URL() const { return _URL; }

std::string Request::httpProt() const { return _httpProtocol; }

std::string Request::queryString() const { return _queryString; }

const std::map<std::string, std::string>* Request::headers() const { return &_headers; }

const std::map<std::string, std::string>* Request::cookies() const { return &_cookies; }

std::string Request::host() const { return _host; }

size_t Request::contentLength() const { return _contentLength; }

std::string Request::contentType() const { return _contentType; }

std::string Request::directory() const { return _directory; }

std::string Request::file() const { return _file; }

bool Request::dirListing() const
{
	if (_locInfo->second.dir_listing == "yes")
		return true;
	else if (_locInfo->second.dir_listing == "no")
		return false;
	else if (!_activeConfig->getDefaultDirlisting())
		return false;
	else
		return true;
}

bool Request::cgiRequest() const { return _cgiRequest; }

dynCont Request::internalScript() const { return _internalScript; }

bool Request::setCookie() const { return _setCookie; }

std::string Request::sessionID() const { return _sessionID; }

std::string Request::standardFile() const { return _standardFile; }

std::string Request::updatedDir() const { return _updatedDirectory; }

std::string Request::updatedURL() const { return _updatedURL; }

std::string Request::statusPagePath(int code) const
{
	std::map<int, std::string>::const_iterator codePath = _activeConfig->getStatusPagePaths()->find(code);
	
	if (codePath == _activeConfig->getStatusPagePaths()->end())
		return "";
	return prependRoot(codePath->second);
}

std::string Request::root() const { return _activeConfig->getRoot(); }

const std::map<std::string, std::string>*	Request::mimeTypes() const { return _activeConfig->getMIMETypes(); }

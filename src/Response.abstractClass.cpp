#include "../include/Response.abstractClass.hpp"

Response::Response(const Request& request):
	_request(request)
{}

// dynamic content generation constructor (w/o CGI)
Response::Response(generateContent genContent, const Request& request):
	_request(request)
{
	sendSelector = &Response::sendInternalBuffer;
	switch (genContent)
	{
		case dirListing:
			generateDirListingPage();
			break;
		case sessionLog:			
			generateSessionLogPage();
			break;
		default:
			throw (ErrorCode(500, __FUNCTION__));
	}
	_contentLength = _dynContBuffer.str().size();
	_contentType = getMimeType(".html");
	_sendBuffer	<< buildResponseHead(200);
	_sendBuffer << _dynContBuffer.str();
}

Response::Response(const Response& src):
	_request(src._request)
{
	*this = src;
}

Response& Response::operator=(const Response& src)
{
	_filePosition = src._filePosition;
	_sendPath = src._sendPath;
	_contentLength = src._contentLength;
	_contentType = src._contentType;
	_responseHeadSent = src._responseHeadSent;
	_sendBuffer << src._sendBuffer.str();
	_dynContBuffer << src._dynContBuffer.str();
	return *this;
}

bool Response::send(int fd)
{
	std::cout << "Response::send" << std::endl;
	return (this->*sendSelector)(fd);
}

bool Response::sendInternalBuffer(int fd)
{
	char buffer[SEND_CHUNK_SIZE];
	_sendBuffer.read(buffer, SEND_CHUNK_SIZE);

	std::cout << "\n" << __FUNCTION__ << " on fd: " << fd << std::endl;
	std::cout << _sendBuffer.str() << std::endl;
	
	if (::send(fd, buffer, _sendBuffer.gcount(), 0) <= 0)
		throw NetworkFailure(__FUNCTION__);
	
	if (_sendBuffer.tellg() == std::streampos(-1)) // end of buffer reached
		return false;
	return true;
}

bool Response::sendFile(int fd)
{
	ANNOUNCEME
	if (!_responseHeadSent)
	{
		std::cout << "stdfile in sendfile: " << _request.standardFile() << std::endl;
		std::cout << "sendpath in sendfile: " << _sendPath << std::endl;
		_contentLength = fileSize(_sendPath);
		_contentType = getMimeType(_sendPath);
		_sendBuffer << buildResponseHead(200);
		sendInternalBuffer(fd); // if sendchunk is smaller than the header, this will not work
		_responseHeadSent = true;
		return true;
	}
	std::ifstream fileStream(_sendPath.c_str(), std::ios::binary);
	if (!fileStream)
	{
		fileStream.close();
		throw ErrorCode(500, __FUNCTION__);
	}
	
	char buffer[SEND_CHUNK_SIZE];
	fileStream.seekg(_filePosition);
	fileStream.read(buffer, SEND_CHUNK_SIZE);

	if (::send(fd, buffer, fileStream.gcount(), 0) <= 0)
	{
		fileStream.close();
		throw NetworkFailure(__FUNCTION__);
	}
	if (fileStream.eof())
	{
		fileStream.close();
		return false;
	}
	_filePosition = fileStream.tellg();
	fileStream.close();
	return true;
}

void Response::generateStatusPageResponse(int code)
{
	std::string httpMsg = getHttpMsg(code);

	_dynContBuffer	<< "<!DOCTYPE html><html><head>\n"
					<< "<title>webserv - " << code << ": " << httpMsg << "</title>\n"
					<< "<style>\n"
					<< "body {background-color: black; color: white; font-family: Arial, sans-serif; margin: 0; padding: 5% 0 0 0; text-align: center;}\n"
					<< "h1 {font-size: 42px;}\n"
					<< "p {font-size: 16px; line-height: 1.5;}\n"
					<< "</style></head>\n"
					<< "<body>\n"
					<< "<h1>" << code << ": " << httpMsg << "</h1>\n"
					<< "<img style=\"margin-left: auto;\" src=\"https://http.cat/" << code << "\" alt=\"" << httpMsg << "\">\n"
					<< "</body>\n"
					<< "</html>\n";
}

void Response::generateSessionLogPage()
{
	std::string logPath = SYS_LOGS + _request.sessionID() + ".log";
	
	std::ifstream logFile(logPath.c_str());
	if (!logFile)
	{
		logFile.close();
		throw ErrorCode(500, __FUNCTION__);
	}

	_dynContBuffer	<< "<!DOCTYPE html>\n"
					<< "<html>\n"
					<< "<head>\n"
					<< "<title>webserv - session log</title>\n"
					<< "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\"/>\n"
					<< "</head>\n"
					<< "<body>\n"
					<< "<div class=\"logContainer\">\n"
					<< "<h2>" << "Log for session id " << _request.sessionID() << "</h2>\n"
					<< "<logtext>" << logFile.rdbuf() << "</logtext>\n"
					<< "</div>\n"
					<< "<img style=\"margin-left: auto; position: fixed; top: 0; right: 0; height: 70%; z-index: 1;\" src=\"/img/catlockHolmes.png\">\n"
					<< "</body>\n"
					<< "</html>\n";
	logFile.close();
}

void Response::generateDirListingPage()
{
	struct dirent*	ent;
	DIR* 			dir = opendir(_request.updatedURL().c_str());

	_dynContBuffer	<< "<head><title>Test Website for 42 Project: webserv</title><link rel=\"stylesheet\" type=\"text/css\" href=\"/styles.css\"/></head>"
					<< "<html><body><h1>Directory Listing</h1><ul>";
	if (dir)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(ent->d_name, ".") == 0)
				continue;
			if (ent->d_type == DT_DIR) // append a slash if it's a directory
				_dynContBuffer << "<li><a href=\"" << _request.directory() + ent->d_name << "/\">" << ent->d_name << "/</a></li>";
			else
				_dynContBuffer << "<li><a href=\"" << _request.directory() + ent->d_name << "\">" << ent->d_name << "</a></li>";
		}
		closedir(dir);
	}
	_dynContBuffer << "</ul></body></html>";
}

std::string Response::buildResponseHead()
{
	std::stringstream	ss;
	
	ss	<< HTTPVERSION << ' ' << _code << ' ' << getHttpMsg(_code) << "\r\n"
		<< "Server: " << SERVERVERSION << "\r\n"
		<< "connection: close" << "\r\n";
	if (_request.cgiRequest())
		ss	<< "transfer-encoding: chunked\r\n"
			<< "content-type: " << getMimeType(".html") << "\r\n"; // CGI only returns html
	else
	{
		ss << "content-length: " << _contentLength << "\r\n";
		if (_contentLength != 0)
			ss << "content-type: " << _contentType << "\r\n";
	}
	if (_request.setCookie())
		ss << buildCookie(SESSIONID, _request.sessionID(), 3600, "/") << "\r\n";
	ss << "\r\n";
	return ss.str();
}

std::string Response::getMimeType(const std::string& filepath)
{
	std::string 										extension = fileExtension(filepath);
	std::map<std::string, std::string>::const_iterator	it = _request.mimeTypes()->find(extension);

	if (it == _request.mimeTypes()->end())
		return "application/octet-stream";
	return it->second;
}

std::string Response::buildCookie(const std::string& key, const std::string& value, int expiration, const std::string& path)
{
	std::stringstream cookie;

	cookie << "set-cookie: " << key << "=" << value << ";";
	if (expiration >= 0)
		cookie << "max-age=" << expiration << ";";
	cookie << "path=" << path << ";";
	return cookie.str();
}

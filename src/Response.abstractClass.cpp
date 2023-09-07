#include "../include/Response.abstractClass.hpp"

Response::Response(const Request& request):
	_request(request)
{}

Response::Response(const Response& src):
	_request(src._request)
{
	*this = src;
}

Response& Response::operator=(const Response& src)
{
	_code = src._code;
	_contentLength = src._contentLength;
	_contentType = src._contentType;
	_sendBuffer = src._sendBuffer;
	return *this;
}

Response::Response(const Response& src):
	_request(src._request)
{
	*this = src;
}

Response& Response::operator=(const Response& src)
{
	_code = src._code;
	_contentLength = src._contentLength;
	_contentType = src._contentType;
	_sendBuffer << src._sendBuffer.str();
	return *this;
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

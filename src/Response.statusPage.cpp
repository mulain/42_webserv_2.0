#include "../include/Response.statusPage.hpp"

StatusPage::StatusPage(int code, const Request& request):
	Response(request)
{
	std::string responseBody = buildStatusPage();
	
	_code = code;
	_contentLength = responseBody.size();
	_contentType = getMimeType(".html");
	_sendBuffer	<< buildResponseHead();
	_sendBuffer << responseBody;
}

StatusPage::~StatusPage()
{}

bool StatusPage::send(int fd)
{
	char	buffer[SEND_CHUNK_SIZE];
	
	_sendBuffer.read(buffer, SEND_CHUNK_SIZE);
	
	if (::send(fd, buffer, _sendBuffer.gcount(), 0) <= 0)
		throw NetworkFailure(__FUNCTION__);
	
	if (_sendBuffer.tellg() == std::streampos(-1)) // end of buffer reached
		return false;
	return true;
}

std::string StatusPage::buildStatusPage()
{
	std::string 		httpMsg = getHttpMsg(_code);
	std::stringstream	ss;

	ss	<< "<!DOCTYPE html><html><head>\n"
		<< "<title>webserv - " << _code << ": " << httpMsg << "</title>\n"
		<< "<style>\n"
		<< "body {background-color: black; color: white; font-family: Arial, sans-serif; margin: 0; padding: 5% 0 0 0; text-align: center;}\n"
		<< "h1 {font-size: 42px;}\n"
		<< "p {font-size: 16px; line-height: 1.5;}\n"
		<< "</style></head>\n"
		<< "<body>\n"
		<< "<h1>" << _code << ": " << httpMsg << "</h1>\n"
		<< "<img style=\"margin-left: auto;\" src=\"https://http.cat/" << _code << "\" alt=\"" << httpMsg << "\">\n"
		<< "</body>\n"
		<< "</html>\n";
	return ss.str();
}

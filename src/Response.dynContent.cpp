#include "../include/Response.dynContent.hpp"

DynContent::DynContent(dynContent contentSelector, const Request& request):
	Response(request)
{
	std::string responseBody;
	
	switch (contentSelector)
	{
		case dirListing:
			responseBody = generateDirListingPage();
			break;
		case sessionLog:			
			responseBody = buildSessionLogPage();
			break;
		default:
			throw (ErrorCode(500, __FUNCTION__));
	}



	_code = 200;
	_contentLength = responseBody.size();
	_contentType = getMimeType(".html");
	_sendBuffer	<< buildResponseHead();
	_sendBuffer << responseBody;
}

DynContent::DynContent(const DynContent& src):
	Response(src)
{
	// This derived class has no own vars.
}

Response* DynContent::clone() const
{
	return new DynContent(*this);
}

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

std::string Response::buildDirListingPage()
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

std::string DynContent::buildSessionLogPage()
{
	std::string 		logPath = SYS_LOGS + _request.sessionID() + ".log";
	std::stringstream	ss;
	
	std::ifstream logFile(logPath.c_str());
	if (!logFile)
	{
		logFile.close();
		throw ErrorCode(500, __FUNCTION__);
	}

	ss	<< "<!DOCTYPE html>\n"
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
	return ss.str();
}

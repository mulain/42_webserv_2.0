#include "webserv.hpp"

DynContent::DynContent(dynCont contentSelector, const Request& request):
	Response(request)
{
	std::string responseBody;
	
	switch (contentSelector)
	{
		case dirListing:
			responseBody = buildDirListingPage();
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

bool DynContent::send(int fd)
{
	// this derived class only sends from internal buffer, so its public send() just redirects there
	return sendInternalBuffer(fd);
}

Response* DynContent::clone() const
{
	return new DynContent(*this);
}

std::string DynContent::buildDirListingPage()
{
	std::stringstream	ss;
	struct dirent*		ent;
	DIR* 				dir = opendir(_request.updatedURL().c_str());

	ss	<< "<head><title>Test Website for 42 Project: webserv</title><link rel=\"stylesheet\" type=\"text/css\" href=\"/styles.css\"/></head>"
		<< "<html><body><h1>Directory Listing</h1><ul>";
	if (dir)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(ent->d_name, ".") == 0)
				continue;
			if (ent->d_type == DT_DIR) // append a slash if it's a directory
				ss << "<li><a href=\"" << _request.directory() + ent->d_name << "/\">" << ent->d_name << "/</a></li>";
			else
				ss << "<li><a href=\"" << _request.directory() + ent->d_name << "\">" << ent->d_name << "</a></li>";
		}
		closedir(dir);
	}
	ss << "</ul></body></html>";
	return ss.str();
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

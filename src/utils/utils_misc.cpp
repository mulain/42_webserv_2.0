# include "webserv.hpp"

void sigHandler(int sig)
{
	if (sig == SIGINT || sig == SIGTERM) // can only be these, but nicer than voiding
		signum = 1;
}

std::string getHttpMsg(int code)
{
	switch (code)
	{
		case 0: return "No error.";
		
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 102: return "Processing";
		case 103: return "Checkpoint";

		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 208: return "Already Reported";

		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 306: return "Switch Proxy";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";

		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Requested Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 418: return "I'm a teapot";
		case 421: return "Misdirected Request";
		case 422: return "Unprocessable Entity";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 426: return "Upgrade Required";
		case 428: return "Precondition Required";
		case 429: return "Too Many Request";
		case 431: return "Request Header Fields Too Large";
		case 451: return "Unavailable For Legal Reasons";

		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";

		default: return "Unknown Error";
	}
}

std::map<std::string, std::string> initMimeTypesMap()
{
	std::map<std::string, std::string>	mimeTypes;

	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".js"] = "application/javascript";
	mimeTypes[".json"] = "application/json";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".bmp"] = "image/bmp";
	mimeTypes[".ico"] = "image/x-icon";
	mimeTypes[".svg"] = "image/svg+xml";
	mimeTypes[".xml"] = "application/xml";
	mimeTypes[".pdf"] = "application/pdf";
	mimeTypes[".zip"] = "application/zip";
	mimeTypes[".gz"] = "application/gzip";
	mimeTypes[".tar"] = "application/x-tar";
	mimeTypes[".mp4"] = "video/mp4";
	mimeTypes[".mpeg"] = "video/mpeg";
	mimeTypes[".avi"] = "video/x-msvideo";
	mimeTypes[".avif"] = "image/avif";
	mimeTypes[".ogg"] = "audio/ogg";
	mimeTypes[".mp3"] = "audio/mpeg";
	mimeTypes[".wav"] = "audio/wav";
	mimeTypes[".mov"] = "video/quicktime";
	mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
	mimeTypes[".xls"] = "application/vnd.ms-excel";
	mimeTypes[".doc"] = "application/msword";
	mimeTypes[".csv"] = "text/csv";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".rtf"] = "application/rtf";
	mimeTypes[".shtml"] = "text/html";
	mimeTypes[".php"] = "application/php";
	mimeTypes[".jsp"] = "text/plain";
	mimeTypes[".swf"] = "application/x-shockwave-flash";
	mimeTypes[".ttf"] = "application/x-font-truetype";
	mimeTypes[".eot"] = "application/vnd.ms-fontobject";
	mimeTypes[".woff"] = "application/font-woff";
	mimeTypes[".woff2"] = "font/woff2";
	mimeTypes[".ics"] = "text/calendar";
	mimeTypes[".vcf"] = "text/x-vcard";
	mimeTypes[".mid"] = "audio/midi";
	mimeTypes[".midi"] = "audio/midi";
	mimeTypes[".wmv"] = "video/x-ms-wmv";
	mimeTypes[".webm"] = "video/webm";
	mimeTypes[".3gp"] = "video/3gpp";
	mimeTypes[".3g2"] = "video/3gpp2";
	mimeTypes[".pl"] = "text/plain";
	mimeTypes[".py"] = "text/x-python";
	mimeTypes[".java"] = "text/x-java-source";
	mimeTypes[".c"] = "text/x-c";
	mimeTypes[".cpp"] = "text/x-c++";
	mimeTypes[".cs"] = "text/plain";
	mimeTypes[".rb"] = "text/x-ruby";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".shtml"] = "text/html";
	mimeTypes[".xhtml"] = "application/xhtml+xml";
	mimeTypes[".m4a"] = "audio/mp4";
	mimeTypes[".mp4a"] = "audio/mp4";
	mimeTypes[".oga"] = "audio/ogg";
	mimeTypes[".ogv"] = "video/ogg";
	mimeTypes[".ogx"] = "application/ogg";
	mimeTypes[".oga"] = "audio/ogg";
	mimeTypes[".m3u8"] = "application/vnd.apple.mpegurl";
	mimeTypes[".qt"] = "video/quicktime";
	mimeTypes[".ts"] = "video/mp2t";
	mimeTypes[".xl"] = "application/excel";
	mimeTypes[".cab"] = "application/vnd.ms-cab-compressed";
	mimeTypes[".msi"] = "application/x-msdownload";
	mimeTypes[".dmg"] = "application/x-apple-diskimage";
	mimeTypes[".exe"] = "application/octet-stream";
	mimeTypes[".bin"] = "application/octet-stream";
	mimeTypes[".ps"] = "application/postscript";
	mimeTypes[".so"] = "application/octet-stream";
	mimeTypes[".dll"] = "application/octet-stream";
	mimeTypes[".m4v"] = "video/x-m4v";
	mimeTypes[".ser"] = "application/java-serialized-object";
	mimeTypes[".sh"] = "application/x-sh";
	mimeTypes[".log"] = "text/plain";
	mimeTypes[".diff"] = "text/x-diff";
	mimeTypes[".patch"] = "text/x-diff";
	mimeTypes[".xhtml"] = "application/xhtml+xml";
	mimeTypes[".php"] = "application/x-httpd-php";
	mimeTypes[".plist"] = "application/xml";
	mimeTypes[".sln"] = "text/plain";
	mimeTypes[".tiff"] = "image/tiff";
	mimeTypes[".app"] = "application/octet-stream";
	mimeTypes[".ics"] = "text/calendar";
	mimeTypes[".webp"] = "image/webp";

	return mimeTypes;
}

std::string currentTimeCPP98()
{
	time_t		rawtime;
	const char*	timeformat = "%G-%m-%d %H:%M:%S";
	char		timeoutput[69];

	time(&rawtime);
	tm* timeinfo = localtime(&rawtime);
	strftime(timeoutput, 420, timeformat, timeinfo);
	std::string returnVal(timeoutput);
	return returnVal;
}

std::string generateSessionId()
{
	char		sessionID[17];
	const char	charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	size_t		i = 0;
	
	srand(time(NULL));
	for (; i < sizeof(sessionID) - 1; ++i)
		sessionID[i] = charset[rand() % (sizeof(charset) - 1)];
	sessionID[i] = 0;
	return sessionID;
}

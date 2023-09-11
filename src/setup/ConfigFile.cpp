#include "webserv.hpp"

ConfigFile::ConfigFile(const char* configPath)
{
	std::string	configData = loadFile(configPath);
	
	setMIMEtypes();

	if (RECV_CHUNK_SIZE < MAX_REQHEADSIZE)
		throw std::runtime_error(E_CF_SIZEMISMATCH);
	
	while (!configData.empty())
		_configs.push_back(Config(getConfigElement(configData), _mimeTypes));

	if (_configs.empty())
		throw std::runtime_error(E_CF_NOSERVER);

	if (_configs.size() > 10)
		throw std::runtime_error(E_CF_MANYSERVER);
	
	for (size_t i = 0; i < _configs.size(); ++i) // check for bindings on the same host:port
	{
		if (combineSharedNetAddr(_configs[i], i))
			_configs.erase(_configs.begin() + i--);
	}

	std::cout << I_CF_CONFIGIMPORT << std::endl;
}

std::vector<Config> ConfigFile::getConfigs() const
{
	return _configs;
}

std::string ConfigFile::loadFile(const char* path)
{
	std::ifstream		infile(path);
	std::stringstream	buffer;
	std::string			line;
	
	if (!infile)
	{
		std::string invalidpath(path);
		throw std::runtime_error(E_CF_FILEOPEN + invalidpath + '\n');
	}

	while (std::getline(infile, line))
	{
		size_t firstParseChar = line.find_first_not_of(WHITESPACE);
		if (firstParseChar == std::string::npos || line[firstParseChar] == '#')
			continue;
		buffer << line;
	}

	infile.close();
	line = buffer.str();
	trimWhitespace(line);
	return line;
}

std::string ConfigFile::getConfigElement(std::string& configData)
{
	std::string	elementTitle;
	
	elementTitle = splitEraseTrimChars(configData, "{");
	if (elementTitle != SERVER)
		throw std::runtime_error(E_CF_ELMNTDECL + elementTitle + '\n');
	return getInstruction(configData);
}

bool ConfigFile::combineSharedNetAddr(const Config& currentConfig, size_t currentConfigIndex)
{
	for (size_t i = 0; i < currentConfigIndex; ++i)
	{
		if (sharedNetAddr(currentConfig, _configs[i]))
		{
			_configs[i].addAltConfig(currentConfig);
			return true;
		}
	}
	return false;
}

bool ConfigFile::sharedNetAddr(const Config& a, const Config& b)
{
	if (&a == &b)
		return false;
	if (a.getHost() == b.getHost() && a.getPort() == b.getPort())
		return true;
	return false;
}

void ConfigFile::setMIMEtypes()
{
	_mimeTypes[".html"] = "text/html";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".css"] = "text/css";
	_mimeTypes[".js"] = "application/javascript";
	_mimeTypes[".json"] = "application/json";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".bmp"] = "image/bmp";
	_mimeTypes[".ico"] = "image/x-icon";
	_mimeTypes[".svg"] = "image/svg+xml";
	_mimeTypes[".xml"] = "application/xml";
	_mimeTypes[".pdf"] = "application/pdf";
	_mimeTypes[".zip"] = "application/zip";
	_mimeTypes[".gz"] = "application/gzip";
	_mimeTypes[".tar"] = "application/x-tar";
	_mimeTypes[".mp4"] = "video/mp4";
	_mimeTypes[".mpeg"] = "video/mpeg";
	_mimeTypes[".avi"] = "video/x-msvideo";
	_mimeTypes[".avif"] = "image/avif";
	_mimeTypes[".ogg"] = "audio/ogg";
	_mimeTypes[".mp3"] = "audio/mpeg";
	_mimeTypes[".wav"] = "audio/wav";
	_mimeTypes[".mov"] = "video/quicktime";
	_mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
	_mimeTypes[".xls"] = "application/vnd.ms-excel";
	_mimeTypes[".doc"] = "application/msword";
	_mimeTypes[".csv"] = "text/csv";
	_mimeTypes[".txt"] = "text/plain";
	_mimeTypes[".rtf"] = "application/rtf";
	_mimeTypes[".shtml"] = "text/html";
	_mimeTypes[".php"] = "application/php";
	_mimeTypes[".jsp"] = "text/plain";
	_mimeTypes[".swf"] = "application/x-shockwave-flash";
	_mimeTypes[".ttf"] = "application/x-font-truetype";
	_mimeTypes[".eot"] = "application/vnd.ms-fontobject";
	_mimeTypes[".woff"] = "application/font-woff";
	_mimeTypes[".woff2"] = "font/woff2";
	_mimeTypes[".ics"] = "text/calendar";
	_mimeTypes[".vcf"] = "text/x-vcard";
	_mimeTypes[".mid"] = "audio/midi";
	_mimeTypes[".midi"] = "audio/midi";
	_mimeTypes[".wmv"] = "video/x-ms-wmv";
	_mimeTypes[".webm"] = "video/webm";
	_mimeTypes[".3gp"] = "video/3gpp";
	_mimeTypes[".3g2"] = "video/3gpp2";
	_mimeTypes[".pl"] = "text/plain";
	_mimeTypes[".py"] = "text/x-python";
	_mimeTypes[".java"] = "text/x-java-source";
	_mimeTypes[".c"] = "text/x-c";
	_mimeTypes[".cpp"] = "text/x-c++";
	_mimeTypes[".cs"] = "text/plain";
	_mimeTypes[".rb"] = "text/x-ruby";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".shtml"] = "text/html";
	_mimeTypes[".xhtml"] = "application/xhtml+xml";
	_mimeTypes[".m4a"] = "audio/mp4";
	_mimeTypes[".mp4a"] = "audio/mp4";
	_mimeTypes[".oga"] = "audio/ogg";
	_mimeTypes[".ogv"] = "video/ogg";
	_mimeTypes[".ogx"] = "application/ogg";
	_mimeTypes[".oga"] = "audio/ogg";
	_mimeTypes[".m3u8"] = "application/vnd.apple.mpegurl";
	_mimeTypes[".qt"] = "video/quicktime";
	_mimeTypes[".ts"] = "video/mp2t";
	_mimeTypes[".xl"] = "application/excel";
	_mimeTypes[".cab"] = "application/vnd.ms-cab-compressed";
	_mimeTypes[".msi"] = "application/x-msdownload";
	_mimeTypes[".dmg"] = "application/x-apple-diskimage";
	_mimeTypes[".exe"] = "application/octet-stream";
	_mimeTypes[".bin"] = "application/octet-stream";
	_mimeTypes[".ps"] = "application/postscript";
	_mimeTypes[".so"] = "application/octet-stream";
	_mimeTypes[".dll"] = "application/octet-stream";
	_mimeTypes[".m4v"] = "video/x-m4v";
	_mimeTypes[".ser"] = "application/java-serialized-object";
	_mimeTypes[".sh"] = "application/x-sh";
	_mimeTypes[".log"] = "text/plain";
	_mimeTypes[".diff"] = "text/x-diff";
	_mimeTypes[".patch"] = "text/x-diff";
	_mimeTypes[".xhtml"] = "application/xhtml+xml";
	_mimeTypes[".php"] = "application/x-httpd-php";
	_mimeTypes[".plist"] = "application/xml";
	_mimeTypes[".sln"] = "text/plain";
	_mimeTypes[".tiff"] = "image/tiff";
	_mimeTypes[".app"] = "application/octet-stream";
	_mimeTypes[".ics"] = "text/calendar";
	_mimeTypes[".webp"] = "image/webp";
}

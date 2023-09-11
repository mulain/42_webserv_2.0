#include "webserv.hpp"

ConfigFile::ConfigFile(const char* configPath)
{
	std::string	configData = loadFile(configPath);
	
	if (RECV_CHUNK_SIZE < MAX_REQHEADSIZE)
		throw std::runtime_error(E_CF_SIZEMISMATCH);
	
	while (!configData.empty())
		_configs.push_back(Config(getConfigElement(configData)));

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

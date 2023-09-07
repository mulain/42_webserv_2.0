#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include "webserv.hpp"

class ConfigFile
{
	public:
		ConfigFile(const char*);

		const std::vector<Config>*												getConfigs() const;
		const std::map<std::string, std::string>*								getMimeTypes() const;
		const std::map<std::string, void(ConfigFile::*)(std::string&) const>*	getFunctionMap() const;

	private:
		std::string	loadFile(const char*);
		void 		setMIMEtypes();
		std::string	getConfigElement(std::string&);
		bool		combineSharedNetAddr(const Config&, size_t);
		bool		sharedNetAddr(const Config&, const Config&);
		
		std::vector<Config>												_configs;
		std::map<std::string, std::string>								_mimeTypes;
		std::map<std::string, void(ConfigFile::*)(std::string&) const>	_functionMap;
};

#endif

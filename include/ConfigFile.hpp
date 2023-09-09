#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include "webserv.hpp"

class ConfigFile
{
	public:
		ConfigFile(const char*);

		std::vector<Config>			getConfigs() const;

	private:
		std::string							loadFile(const char*);
		void 								setMIMEtypes();
		std::string							getConfigElement(std::string&);
		bool								combineSharedNetAddr(const Config&, size_t);
		bool								sharedNetAddr(const Config&, const Config&);
		
		std::vector<Config>					_configs;
		std::map<std::string, std::string>	_mimeTypes;
};

#endif

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"

class Config
{
	public:
		Config(std::string);

		void										whoIsI() const;
		void										addAltConfig(const Config&);
		const std::vector<std::string>&				getNames() const;
		in_addr_t									getHost() const;
		in_port_t									getPort() const;
		const std::string&							getRoot() const;
		bool										getDefaultDirlisting() const;
		unsigned long								getClientMaxBody() const;
		size_t										getMaxConnections() const;
		const std::string&							getStandardFile() const;
		const std::map<int, std::string>*			getStatusPagePaths() const;
		const std::map<std::string, s_locInfo>*		getLocations() const;
		const std::map<std::string, std::string>*	getCgiPaths() const;
		const std::vector<Config>&					getAltConfigs() const;

	private:
		void										setFunctionMap();
		void										parseNames(std::string&);
		void										parseHost(std::string&);
		void										parsePort(std::string&);
		void										parseRoot(std::string&);
		void										parseDirListing(std::string&);
		void										parseClientMaxBody(std::string&);
		void										parseMaxConnections(std::string&);
		void										parseStandardFile(std::string&);
		void										parseStatusPagePaths(std::string&);
		void										parseLocations(std::string&);
		void										parseCgiPaths(std::string&);
		
		std::vector<std::string>								_names;
		in_addr_t												_host;
		in_port_t												_port;
		std::string												_root;
		bool													_defaultDirListing;
		unsigned long											_clientMaxBody;
		size_t													_maxConnections;
		std::string												_standardFile;
		std::map<int, std::string>								_statusPagePaths;
		std::map<std::string, s_locInfo>						_locations;
		std::map<std::string, std::string>						_cgiPaths;
		std::vector<Config>										_altConfigs;
		std::map<std::string, void(Config::*)(std::string&)>	_functionMap;
};

#endif

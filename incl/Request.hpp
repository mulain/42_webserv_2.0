#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Request
{
	public:
		Request(std::string&, const Config&, const Client&);
		Request(const Request&);
		
		Request& 											operator=(const Request&);
		void												whoIsI() const;
		void												process();

		const Config*										activeConfig() const;
		std::string											method() const;
		std::string											URL() const;
		std::string											httpProt() const;
		std::string											queryString() const;
		const std::map<std::string, std::string>*			headers() const;
		const std::map<std::string, std::string>*			cookies() const;
		std::string											host() const;
		size_t												contentLength() const;
		std::string											contentType() const;
		std::string											directory() const;
		std::string											file() const;
		bool												dirListing() const;
		bool												cgiRequest() const;
		const std::string&									cgiExecPath() const;
		dynCont												internalScript() const;

		bool												setCookie() const;
		std::string											sessionID() const;
		std::string											standardFile() const;
		std::string											updatedDir() const;
		std::string											updatedURL() const;
		const s_locInfo*									locationInfo() const;
		std::string											statusPagePath(int) const;
		std::string											root() const;
		const std::map<std::string, std::string>*			mimeTypes() const;
	
	private:
		std::string											sayMyName(std::string);
		void												parseRequestLine();
		void												parseRequestHeaders();
		void												handleSessionID();
		void												trackSession();
		void												selectConfig();
		void												requestError();
		void												updateVars();
		std::string											prependRoot(const std::string&) const;
		std::string 										appendSlash(const std::string&);

		std::string*										buffer;
		const Config*										_config;
		const Config*										_activeConfig;
		const Client*										_client;
		std::string											_method;
		std::string											_URL;
		std::string											_httpProtocol;
		std::string											_queryString;
		std::map<std::string, std::string>					_headers;
		std::map<std::string, std::string>					_cookies;
		std::string											_host;
		unsigned long										_contentLength;
		std::string											_contentType;
		std::string											_sessionID;
		std::string											_directory;
		std::string											_file;
		bool												_cgiRequest;
		dynCont												_internalScript;
		bool												_setCookie;
		std::string											_standardFile;
		std::string											_updatedDirectory;
		std::string											_updatedURL;
		s_locInfo											_locationInfo;
		std::string											_cgiExecPath;
};

#endif

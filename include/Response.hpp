#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Response
{
	public:
		Response(int, const Request&); // send a status page
		Response(std::string, const Request&); // send a file from disk
		Response(dynContent, const Request&); // generate and send from internal buffer
		Response(const Response&);

		Response&			operator=(const Response&);
		bool				send(int);
	
	private:
		bool				(Response::*sendSelector)(int);
		bool				sendFile(int);
		bool				sendInternalBuffer(int);
		void				generateStatusPageResponse(int);
		void				generateSessionLogPage();
		void				generateDirListingPage();
		std::string			buildResponseHead(int);
		std::string			getMimeType(const std::string&);
		std::string			buildCookie(const std::string&, const std::string&, int, const std::string&);
		
		const Request&		_request;
		std::streampos		_filePosition;
		std::string			_sendPath;
		size_t				_contentLength;
		std::string			_contentType;
		bool				_responseHeadSent;
		std::stringstream	_sendBuffer;
		std::stringstream	_dynContBuffer;
};

#endif

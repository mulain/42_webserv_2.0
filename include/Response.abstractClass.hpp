#ifndef RESPONSE_ABSTRACT_HPP
# define RESPONSE_ABSTRACT_HPP

# include "webserv.hpp"

class Response
{
	public:
		Response(const Request&);
		Response(int, const Request&); // status page constructor
		Response(std::string, const Request&); // send file constructor
		Response(generateContent, const Request&); // send internal buffer constructor
		Response(const Response&);
		virtual ~Response();

		Response&			operator=(const Response&);
		virtual bool		send(int) = 0;
	
	protected:
//		bool				sendFile(int);
//		bool				sendInternalBuffer(int);
		void				generateSessionLogPage();
		void				generateDirListingPage();
		std::string			buildResponseHead();
		std::string			getMimeType(const std::string&);
		std::string			buildCookie(const std::string&, const std::string&, int, const std::string&);
		
		const Request&		_request;
		int					_code;
		size_t				_contentLength;
		std::string			_contentType;
		std::stringstream	_sendBuffer;
};

#endif

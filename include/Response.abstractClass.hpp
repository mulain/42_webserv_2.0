#ifndef RESPONSE_ABSTRACT_HPP
# define RESPONSE_ABSTRACT_HPP

# include "webserv.hpp"

class Response
{
	public:
		Response(dynContent, const Request&); // send internal buffer constructor
		Response(const Response&);
		virtual ~Response() {};

		Response&			operator=(const Response&);
		virtual Response*	clone() const = 0; // when overriding these in derived class, should use "override" even if not necessary. But not cpp98.
		virtual bool		send(int) = 0;
	
	protected:
		Response(const Request&);
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

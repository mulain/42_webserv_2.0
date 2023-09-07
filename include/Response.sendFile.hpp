#ifndef RESPONSE_SENDFILE_HPP
# define RESPONSE_SENDFILE_HPP

# include "webserv.hpp"

class SendFile: public Response
{
	public:
		SendFile(std::string, const Request&);
		~SendFile();

		bool				send(int); // override (but not cpp98)
	
	private:
		void				sendResponseHead(int fd);
		
		std::string			_sendPath;
		bool				_responseHeadSent;
		std::streampos		_filePosition;
};

#endif

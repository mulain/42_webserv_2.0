#ifndef SENDFILE_RESPONSE_HPP
# define SENDFILE_RESPONSE_HPP

# include "webserv.hpp"

class SendFile: public Response
{
	public:
		SendFile(int, std::string, const Request&);
		SendFile(const SendFile&);
		~SendFile() {}

		bool				send(int);
		Response*			clone() const;
	
	private:
		std::string			_sendPath;
		bool				_responseHeadIncomplete;
		std::streampos		_filePosition;
};

#endif

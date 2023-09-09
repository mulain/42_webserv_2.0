#ifndef RESPONSE_SENDFILE_HPP
# define RESPONSE_SENDFILE_HPP

# include "webserv.hpp"

class SendFile: public Response
{
	public:
		SendFile(std::string, int, const Request&);
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

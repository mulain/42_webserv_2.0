#ifndef ERRORCODE_EXCEPTION_HPP
# define ERRORCODE_EXCEPTION_HPP

# include "webserv.hpp"

class ErrorCode: public std::exception
{
	public:
		ErrorCode(int, std::string);
		virtual ~ErrorCode() throw();

		const char*	what() const throw();
		int			getCode() const;

	private:
		int				_errorCode;
		std::string		_errorMsg;
};

#endif

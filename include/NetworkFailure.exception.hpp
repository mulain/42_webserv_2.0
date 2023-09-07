#ifndef NETWORKFAILURE_EXCEPTION_HPP
# define NETWORKFAILURE_EXCEPTION_HPP

# include "webserv.hpp"

class NetworkFailure: public std::exception
{
	public:
		NetworkFailure(std::string errorMsg);
		virtual ~NetworkFailure() throw();

		const char*	what() const throw();

	private:
		std::string		_errorMsg;
};

#endif

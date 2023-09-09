#include "webserv.hpp"

CloseConnection::CloseConnection(std::string throwingFunction, std::string event)
{
	_errorMsg = "Client termination exception in " + throwingFunction +  ": " + event;
}

CloseConnection::~CloseConnection() throw() {}

const char* CloseConnection::what() const throw()
{
	return _errorMsg.c_str();
}

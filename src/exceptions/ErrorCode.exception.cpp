#include "webserv.hpp"

ErrorCode::ErrorCode(int errorCode, std::string throwingFunction):
	_errorCode(errorCode)
{
	std::stringstream ss;
	
	ss << "\n" << throwingFunction << ": " << errorCode << " " << getHttpMsg(errorCode);
	_errorMsg = ss.str();
}

ErrorCode::~ErrorCode() throw() {}

const char* ErrorCode::what() const throw() { return _errorMsg.c_str(); }

int ErrorCode::getCode() const { return _errorCode; }

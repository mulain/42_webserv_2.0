#ifndef RESPONSE_DIRLISTING_HPP
# define RESPONSE_DIRLISTING_HPP

# include "webserv.hpp"

class DynContent: public Response
{
	public:
		DynContent(dynContent, const Request&);
		DynContent(const DynContent&);
		~DynContent() {}

		bool				send(int);
		Response*			clone() const;
	
	private:
		std::string			buildSessionLogPage();
};

#endif

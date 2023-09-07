#ifndef RESPONSE_STATUSPAGE_HPP
# define RESPONSE_STATUSPAGE_HPP

# include "webserv.hpp"

class StatusPage: public Response
{
	public:
		StatusPage(int, const Request&);
		~StatusPage();

		bool				send(int); // override (but not cpp98)
	
	private:
		std::string			buildStatusPage();
};

#endif

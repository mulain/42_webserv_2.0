#ifndef RESPONSE_STATUSPAGE_HPP
# define RESPONSE_STATUSPAGE_HPP

# include "Response.abstractClass.hpp"

class StatusPage: public Response
{
	public:
		StatusPage(int, const Request&);
		StatusPage(const StatusPage&);
		~StatusPage() {}

		bool				send(int);
		Response*			clone() const;
	
	private:
		std::string			buildStatusPage();
};

#endif

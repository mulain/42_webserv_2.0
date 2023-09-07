#ifndef BINDING_HPP
# define BINDING_HPP

# include "webserv.hpp"

class Binding
{
	public:
		Binding(const Config&);

		void			whoIsI();
		int				fd();
		void			setFd(int);
		sockaddr_in*	addrOfSocketAddr();
	
	private:
		const Config*	_config;
		int				_fd;
		sockaddr_in		_socketAddr;		
};

#endif

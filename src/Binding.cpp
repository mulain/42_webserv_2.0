# include "webserv.hpp"

Binding::Binding(const Config& config):
	_config(&config)
{
	_fd = -1;
	_socketAddr.sin_family = AF_INET;
	_socketAddr.sin_addr.s_addr = config.getHost();
	_socketAddr.sin_port = config.getPort();
}

void Binding::whoIsI()
{
	std::stringstream title;
	std::string dashes(20, '-');
	title << dashes << " Binding " << inet_ntoa(_socketAddr.sin_addr) << ":" << ntohs(_config->getPort()) << " on fd " << _fd << " " << dashes;
	std::string separator(title.str().size(), '-');
	
	std::cout << "\n" << title.str() << "\n";
	_config->whoIsI();
	std::cout << separator << "\n" << std::endl;
}

void Binding::setFd(int newFd) { _fd = newFd; }

int Binding::fd() { return _fd; }

sockaddr_in* Binding::addrOfSocketAddr() { return &_socketAddr; }
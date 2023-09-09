# include "webserv.hpp"

Server::Server(int argc, char** argv)
{
	std::string path = "system/configs/example.conf";
	if (argc > 1)
		path = argv[1];
	
	ConfigFile	configFile(path.c_str());
	
	_configs = configFile.getConfigs();
}

void Server::launchBindings()
{
	for (size_t i = 0; i < _configs.size(); ++i)
	{
		try
		{
			bindListeningSocket((_configs)[i]);
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	if (_bindings.empty())
	{
		std::cerr << E_S_NOBINDINGS << std::endl;
		shutdown();
	}
	std::cout << I_S_BINDINGS << std::endl;
}

void Server::bindListeningSocket(const Config& config)
{
	Binding newBinding(config);
	int	options = 1;
	int	listen_fd;
	
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		closeFdAndThrow(listen_fd);

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&options, sizeof(options)) == -1)
		closeFdAndThrow(listen_fd);

	if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1)
		closeFdAndThrow(listen_fd);

	if (bind(listen_fd, (struct sockaddr*) newBinding.addrOfSocketAddr(), sizeof(sockaddr_in)) == -1)
		closeFdAndThrow(listen_fd);

	if (listen(listen_fd, SOMAXCONN) == -1)
		closeFdAndThrow(listen_fd);
	
	newBinding.setFd(listen_fd);
	_bindings.push_back(newBinding);
	addPollStruct(listen_fd, POLLIN);
	_bindings.back().whoIsI();
}

bool Server::poll()
{
	if (::poll(_pollStructs.data(), _pollStructs.size(), -1) == -1)
	{
		if (!signum)
			perror(__FUNCTION__);
	}
	if (signum)
		return false;
	return true;
}

void Server::acceptClients()
{
	for (size_t i = 0; i < _bindings.size(); ++i)
	{
		if (_bindings[i].fd() != _pollStructs[i].fd)
			std::cout << MMMMMEGAERROR << std::endl;
		
		if (!(_pollStructs[i].revents & POLLIN))
			continue;
			
		while (true)
		{
			sockaddr_in	addr;
			socklen_t	clientAddrSize = sizeof(addr);
		
			int new_sock = accept(_pollStructs[i].fd, (sockaddr*)&addr, &clientAddrSize);
			if (new_sock == -1)
			{
				if (errno == EWOULDBLOCK)
					return;
				closeFdAndThrow(new_sock);
			}
			if (fcntl(new_sock, F_SETFL, O_NONBLOCK) == -1)
				closeFdAndThrow(new_sock);

			addPollStruct(new_sock, POLLIN | POLLHUP);
			_clients.push_back(Client(_configs[i], _pollStructs.back(), addr));
		}
	}
}

void Server::handleClients()
{
	size_t i = _bindings.size(); // skip listening fds
	while (i < _pollStructs.size())
	{
		try
		{
			_client = getClient(_pollStructs[i].fd);
			_pollStruct = getPollStruct(_pollStructs[i].fd); // pollvectorbegin + i

			if (pollhup())
				continue;

			if (_pollStruct->revents & POLLIN)
				_client->incomingData();
			else if (_pollStruct->revents & POLLOUT)
			{
				if(!_client->outgoingData())
				{
					closeClient(CLOSE_DONE);
					continue;
				}
			}

		}
		catch (const ErrorCode& e)
		{
			std::cerr << e.what() << std::endl;
			_client->sendStatusPage(e.getCode());
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			closeClient(CLOSE_EXCPT);
		}
		++i;
	}
}

bool Server::pollhup()
{
	if (_pollStruct->revents & POLLHUP)
	{
		closeClient(CLOSE_POLLHUP);
		return true;
	}
	return false;
}

void Server::closeClient(std::string msg)
{
	if (msg.empty())
		msg = "no information given.";
	std::cout << "\nClosing Client on fd " << _client->getFd() << ": " << msg << std::endl;
	close(_client->getFd());
	
	_pollStructs.erase(_pollStruct);
	_clients.erase(_client);
}

std::vector<Client>::iterator Server::getClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	
	while (it != _clients.end() && it->getFd() != fd)
		++it;
	if (it == _clients.end())
	{
		std::cerr << E_S_CLIENTNOTFOUND << std::endl;
		throw std::runtime_error(__FUNCTION__);
	}
	return it;
}

std::vector<pollfd>::iterator Server::getPollStruct(int fd)
{
	std::vector<pollfd>::iterator it = _pollStructs.begin();
		
	while (it != _pollStructs.end() && it->fd != fd)
		++it;
	if (it == _pollStructs.end())
	{
		std::cerr << E_S_PSTRUCNOTFOUND << std::endl;
		throw std::runtime_error(__FUNCTION__);
	}
	return it;
}

void Server::addPollStruct(int fd, short flags)
{
	pollfd new_pollStruct;
	new_pollStruct.fd = fd;
	new_pollStruct.events = flags;
	new_pollStruct.revents = 0;
	_pollStructs.push_back(new_pollStruct);
}

void Server::closeFdAndThrow(int fd)
{
	if (fd != -1)
		close(fd);
	throw std::runtime_error(strerror(errno));
}

void Server::shutdown()
{
	std::cout << "\nShutdown." << std::endl;
	
	for (std::vector<pollfd>::iterator it = _pollStructs.begin(); it != _pollStructs.end(); ++it)
	{
		std::cout << "Closing socket fd " << it->fd << "." << std::endl;
		close(it->fd);
	}
}

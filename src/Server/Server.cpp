# include "webserv.hpp"

Server::Server(int argc, char** argv)
{
	std::string	path = "system/configs/example.conf";
	
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
	Binding*	newBinding = new Binding(config);
	int			options = 1;
	int			listen_fd;
	
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		bindError(listen_fd, newBinding);

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&options, sizeof(options)) == -1)
		bindError(listen_fd, newBinding);

	if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1)
		bindError(listen_fd, newBinding);

	if (bind(listen_fd, (struct sockaddr*) newBinding->addrOfSocketAddr(), sizeof(sockaddr_in)) == -1)
		bindError(listen_fd, newBinding);

	if (listen(listen_fd, SOMAXCONN) == -1)
		bindError(listen_fd, newBinding);
	
	addPollStruct(listen_fd, POLLIN);
	newBinding->setFd(listen_fd);
	_bindings.push_back(newBinding);
	newBinding->whoIsI();
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
		if (!(_pollStructs[i].revents & POLLIN))
			continue;
			
		while (true)
		{
			sockaddr_in	addr;
			socklen_t	addrSize = sizeof(addr);
			int			new_sock;
		
			new_sock = accept(_pollStructs[i].fd, (sockaddr*)&addr, &addrSize);
			if (new_sock == -1)
			{
				if (errno == EWOULDBLOCK)
					return;
				acceptError(new_sock);
			}
			
			if (fcntl(new_sock, F_SETFL, O_NONBLOCK) == -1)
				acceptError(new_sock);

			addPollStruct(new_sock, POLLIN | POLLHUP);
			_clients.push_back(new Client(_configs[i], new_sock, addr));
		}
	}
}

void Server::handleClients()
{
	_pollStruct = _pollStructs.begin() + _bindings.size();

	while (_pollStruct != _pollStructs.end())
	{
		try
		{
			_client = getClient(_pollStruct->fd);

			if (pollhup())
				continue;

			if (pollin())
				continue;

			if (pollout())
				continue;
		}
		catch (const ErrorCode& e)
		{
			std::cerr << e.what() << std::endl;
			(*_client)->sendStatusPage(e.getCode());
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			closeClient(CLOSE_EXCPT);
			continue;
		}
		++_pollStruct;
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

bool Server::pollin()
{
	if (_pollStruct->revents & POLLIN)
	{
		(*_client)->incomingData(_pollStruct);
		++_pollStruct;
		return true;
	}
	return false;
}

bool Server::pollout()
{
	if (_pollStruct->revents & POLLOUT)
	{
		if (!(*_client)->outgoingData())
		{
			closeClient(CLOSE_DONE);
			return true;
		}
	}
	return false;
}

void Server::closeClient(std::string msg)
{
	size_t	index = _pollStruct - _pollStructs.begin();
	
	std::cout << "\nClosing Client on fd " << _pollStruct->fd << ": " << msg << std::endl;
	close (_pollStruct->fd);
	
	_pollStructs.erase(_pollStruct);
	_pollStruct = _pollStructs.begin() + index;
	
	delete *_client;
	_clients.erase(_client);
}

std::vector<Client*>::iterator Server::getClient(int fd)
{
	std::vector<Client*>::iterator it = _clients.begin();
	
	while (it != _clients.end() && (*it)->getFd() != fd)
		++it;
	if (it == _clients.end())
	{
		std::cerr << E_S_CLIENTNOTFOUND << std::endl;
		throw std::runtime_error(__FUNCTION__);
	}
	return it;
}

void Server::addPollStruct(int fd, short flags)
{
	pollfd	newPollStruct;
	
	newPollStruct.fd = fd;
	newPollStruct.events = flags;
	newPollStruct.revents = 0;
	
	_pollStructs.push_back(newPollStruct);
}

void Server::bindError(int fd, Binding* newBinding)
{
	if (fd != -1)
		close(fd);
	delete newBinding;
	throw std::runtime_error(strerror(errno));
}

void Server::acceptError(int fd)
{
	if (fd != -1)
		close(fd);
	throw std::runtime_error(strerror(errno));
}

void Server::shutdown()
{
	std::cout << "\nShutdown." << std::endl;
	
	std::cout << "\nClosing " << _pollStructs.size() << (_pollStructs.size() == 1 ? " socket:" : " sockets:") << std::endl;
	for (std::vector<pollfd>::iterator it = _pollStructs.begin(); it != _pollStructs.end(); ++it)
	{
		std::cout << "\tClosing socket fd " << it->fd << "." << std::endl;
		close(it->fd);
	}
	
	std::cout << "\nDeleting " << _bindings.size() << (_bindings.size() == 1 ? " Binding:" : " Bindings:") << std::endl;
	for (size_t i = 0; !_bindings.empty(); ++i)
	{
		std::cout << "\tDeleting Binding " << i << "." << std::endl;
		delete _bindings[0];
		_bindings.erase(_bindings.begin());
	}

	if (_clients.empty())
		std::cout << "\nNo Clients connected, nothing to delete." << std::endl;
	else
	{
		std::cout << "\nDeleting " << _clients.size() << (_clients.size() == 1 ? " Client:" : " Clients:") << std::endl;
		for (size_t i = 0; !_clients.empty(); ++i)
		{
			std::cout << "\tDeleting Client " << i << "." << std::endl;
			delete _clients[0];
			_clients.erase(_clients.begin());
		}
	}
}

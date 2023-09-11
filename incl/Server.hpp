#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

class Server
{
	public:
		Server(int, char**);

		void launchBindings();
		bool poll();
		void acceptClients();
		void handleClients();
		void shutdown();

	private:
		bool							pollhup();
		
		void							bindListeningSocket(const Config&);
		void							bindError(int, Binding*);
		void							addPollStruct(int fd, short flags);
		void							acceptError(int);
		void							closeClient(std::string);
		std::vector<Client*>::iterator	getClient(int);

		std::vector<Binding*>			_bindings;
		std::vector<pollfd>				_pollStructs;
		std::vector<Config>				_configs;
		std::vector<Client*>			_clients;
		
		std::vector<Client*>::iterator	_client;
		std::vector<pollfd>::iterator	_pollStruct;
		
};

#endif

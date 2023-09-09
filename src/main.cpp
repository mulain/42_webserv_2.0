#include "webserv.hpp"

volatile sig_atomic_t sigInt = 0;

int main(int argc, char** argv)
{
	std::signal(SIGINT, sigHandler);
	
	Server webserv(argc, argv);
	
	webserv.launchBindings();
	
	while (webserv.poll())
	{
		std::cout << "start of main loop controlled by poll" << std::endl;
		try
		{
			webserv.acceptClients();
			webserv.handleClients();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	webserv.shutdown();
}

#include "webserv.hpp"

volatile sig_atomic_t 				signum = 0;
std::map<std::string, std::string>	mimeTypes = initMimeTypesMap();

int main(int argc, char** argv)
{
	std::signal(SIGINT, sigHandler);
	std::signal(SIGTERM, sigHandler);
	
	Server	webserv(argc, argv);
	
	webserv.launchBindings();
	
	while (webserv.poll())
	{
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

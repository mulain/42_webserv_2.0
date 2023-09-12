#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"

class Client
{
	public:
		Client(const Config&, int, sockaddr_in);
		Client(const Client&);
		~Client();
		
		Client&							operator=(const Client&);

		void 							whoIsI() const;
		void							incomingData(std::vector<pollfd>::iterator);
		bool							outgoingData();
		void							sendStatusPage(int); // called by ErrorCode catch
		int								getFd() const;
		const char*						getAddr() const;

	private:
		std::string						sayMyName(std::string);
		void							receive();
		void							newRequest();
		void							newResponse(int);
		void							newResponse(std::string);
		void							newResponse(dynCont);

		void							handleCGI();
		void							handleGet();
		void							handleDelete();
		void							handlePost();

		void							setCgiFiles();
		void							launchChild();
		void							buildArgvEnv();
		void							cgiError();
		void							childError();
		void							closeFd(int*);

		std::string						_buffer;
		const Config&					_config;
		Request*						_request;
		Response*						_response;
		int								_fd;
		sockaddr_in						_address;
		std::vector<pollfd>::iterator	_pollStruct;
		
		// POST
		bool							_append;
		size_t							_bytesWritten;
 
		// CGI
		pid_t							_cgiPid;
		time_t							_childBirth;
		int								_parentToChild[2];
		int								_childToParent[2];

		std::string						_cgiIn;
		std::string						_cgiOut;

		std::vector<std::string>		_envVec;
		std::vector<char*>				_env;
		std::vector<std::string>		_argvVec;
		std::vector<char*>				_argv;
};

#endif
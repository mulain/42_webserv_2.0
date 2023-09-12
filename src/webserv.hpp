#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// external headers
# include <fstream>
# include <iostream>
# include <sstream>

# include <cstdio>
# include <cstdlib>
# include <csignal>
# include <cstring>

# include <algorithm>
# include <exception>
# include <list>
# include <map>
# include <string>
# include <vector>

# include <arpa/inet.h>
# include <errno.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <sys/poll.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
# include <dirent.h>

// forward declarations
class Binding;
class Client;
class Config;
class ConfigFile;
class Request;
class Server;

// typedefs

typedef struct locationInformation
{
	bool			get;
	bool			post;
	bool			delete_;
	std::string		dir_listing;
	std::string		http_redir;
	std::string		upload_dir;
	std::string		std_file;
}	locInfo;

typedef enum dynamicContentSelector
{
	no,
	statusPage,
	dirListing,
	sessionLog
}	dynCont;


// internal headers
# include "defines.hpp"

// exceptions
# include "ErrorCode.exception.hpp"
# include "CloseConnection.exception.hpp"

// Response
# include "A_Response.hpp"
# include "DynContent.Response.hpp"
# include "File.Response.hpp"

// setup
# include "Config.hpp"
# include "ConfigFile.hpp"

# include "Binding.hpp"
# include "Request.hpp"
# include "Client.hpp"
# include "Server.hpp"

# include "utils.hpp"

// global vars

extern volatile sig_atomic_t				signum;
extern std::map<std::string, std::string>	mimeTypes;

#endif

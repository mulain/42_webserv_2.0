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

// internal headers
# include "typedefs.hpp"
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

// global vars
extern volatile sig_atomic_t signum;

// utils_file.cpp
bool		resourceExists(const std::string&);
bool		isDirectory(const std::string&);
size_t		fileSize(const std::string&);
std::string	fileExtension(const std::string&);

// utils_misc.cpp
void								sigHandler(int);
std::string							getHttpMsg(int);
std::map<std::string, std::string>	initMimeTypesMap();
std::string							currentTimeCPP98();
std::string							generateSessionId();

// utils_string.cpp
bool								isAlnumStr(const std::string &);
bool								isAlnumStrPlus(const std::string&, const std::string&);
bool								isSameNoCase(const std::string&, const std::string&);
bool								isStringInVec(const std::string&, const std::vector<std::string>&);
std::string							strToLower(const std::string&);
std::string							trimWhitespace(std::string&);
std::string							splitEraseStr(std::string &, const std::string&);
std::string							splitEraseTrimChars(std::string&, const std::string&);
std::vector<std::string>			splitEraseStrVec(std::string&, const std::string&, const std::string&);
std::string							getInstruction(std::string&);
std::map<std::string, std::string>	parseStrMap(std::string&, const std::string&, const std::string&, const std::string&);

#endif

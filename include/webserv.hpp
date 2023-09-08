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
//class Response;
class Server;

// internal headers
# include "typedefs.hpp"
# include "defines.hpp"
# include "Binding.hpp"
# include "Request.hpp"

# include "Response.abstractClass.hpp"
# include "Response.dynContent.hpp"
# include "Response.sendFile.hpp"
# include "Response.statusPage.hpp"

# include "ErrorCode.exception.hpp"
# include "NetworkFailure.exception.hpp"
# include "Client.hpp"
# include "Config.hpp"
# include "ConfigFile.hpp"
# include "Server.hpp"

// global vars
extern volatile sig_atomic_t sigInt;

// Captures SIGINT and sets the corresponding global variable to true.
void sigHandler(int);

// Determines whether a string contains only alphanumerical characters.
bool isAlnumStr(const std::string &);

// Determines whether a string contains only alphanumerical characters or characters from the 2nd argument.
bool isAlnumStrPlus(const std::string&, const std::string&);

// Determines whether strings are the same (case-insensitive).
bool isSameNoCase(std::string, std::string);

// Converts the alphabetical characters in a string to lowercase.
std::string strToLower(std::string);

// Removes the characters defined by the macro WHITESPACE from the beginning and end of a string ref.
std::string trim(std::string &);

// Returns a substring from the beginning of the passed string ref to the beginning of the first occurence of the 2nd argument. Deletes the substring and the 2nd argument from the passed string.
std::string splitEraseStr(std::string &, const std::string&);

// Returns a substring from the beginning of the passed string ref to the first occurence of any char from the 2nd argument. Deletes the substring from the passed string, but not the encountered delimiting char. Calls trim function on both the remainder of the string ref and the returned element.
std::string splitEraseTrimChars(std::string&, const std::string&);

// Returns a string vector. First argument is the string ref to operate on. 2nd argument is a string containing the characters of which any single one delimits the final strings. The 3rd argument denotes the end of the region to be processed.
std::vector<std::string> splitEraseStrVec(std::string& input, const std::string& targetChars, const std::string& endOfParsing);

// Splits a string ref and returns the first instruction it contains, defined as all characters from the string ref's start until the next semicolon or within the next pair of curly braces. Deletes the instruction from the string ref.
std::string getInstruction(std::string& inputStr);

// Checks whether a resource exists, be it file or directory.
bool resourceExists(const std::string&);

// Checks whether a path corresponds to a directory
bool isDirectory(const std::string&);

// Returns the size of a file. Returns (size_t)-1 on error.
size_t fileSize(const std::string&);

// Returns the message string associated with an HTTP status code.
std::string getHttpMsg(int);

// Checks whether a string is present in a string vector.
bool stringInVec(const std::string&, const std::vector<std::string>&);

// From the passed string, returns the last '.' and the characters behind it, e.g. ".html".
std::string fileExtension(const std::string&);

// Throws a runtime error with the message contained in errnum and closes the passed fd if it is not -1.
void closeFdAndThrow(int);

// Operates on a string ref that contains key-value pairs. 1st arg is the string ref. 2nd is end of key, 3rd is end of value, 4th is end of region to parse in 1st arg. Erases parsed region from the string ref.
std::map<std::string, std::string> parseStrMap(std::string&, const std::string&, const std::string&, const std::string&);

// Returns the current time in a pleasant format without using modern CPP.
std::string currentTime();

// Generates a random string to represent a session.
std::string generateSessionId();

#endif

#ifndef UTILS_HPP
# define UTILS_HPP

# include "webserv.hpp"

// utils_file.cpp
bool		resourceExists(const std::string&);
bool		isDirectory(const std::string&);
size_t		fileSize(const std::string&);
std::string	fileExtension(const std::string&);

// utils_misc.cpp
void								sigHandler(int);
std::string							getHttpMsg(int);
std::map<std::string, std::string>	initMimeTypesMap();
std::string							getMimeType(const std::string&);
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

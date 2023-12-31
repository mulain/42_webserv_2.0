#ifndef DEFINES_HPP
# define DEFINES_HPP

// general macros
# define SERVERVERSION	"webserv v2.0"
# define WHITESPACE		" \t\v\r\n"
# define HTTPVERSION	"HTTP/1.1"
# define MYNAME			prependClassName(__FUNCTION__)
# define ANNOUNCEME		std::cout << "Announcer: " << __FUNCTION__ << std::endl;
# define ANNOUNCEME_FD	std::cout << "Announcer: " << __FUNCTION__ << " on fd " << getFd() << std::endl;
# define GOODBYE		std::cout << "Now leaving: " << __FUNCTION__ << std::endl;	
# define SESSIONID		"sessionid"

// paths
# define SYS_TEMP_CGIIN		"system/temp/cgiIn_fd"
# define SYS_TEMP_CGIOUT	"system/temp/cgiOut_fd"
# define SYS_LOGS			"system/logs/"

// numericals
# define RECV_CHUNK_SIZE		16384
# define SEND_CHUNK_SIZE		8192
# define MAX_MAXCLIENTBODY		4294967295 // 2^32 - 1 -> max unsigned long @ 32 bit
# define MAX_MAXCONNECTIONS		1000 // unused as of now
# define MAX_REQHEADSIZE		8192
# define CGI_TIMEOUT			3

// configuration element identifiers
# define SERVER				"server"
# define SERVERNAME			"server_name"
# define HOST				"host"
# define PORT				"listen"
# define ROOT				"root"
# define CLIMAXBODY			"client_max_body"
# define BACKLOG			"backlog"
# define MAXCONNS			"max_connections"
# define STDFILE			"standard_file"
# define UPLOADDIR			"upload_dir"
# define ERRORPAGETITLE		"error_pages"
# define ERRPAGEDEFAULT		"default"
# define LOCATIONTITLE		"location"
# define DIRLISTING			"dir_listing"
# define REDIRECTION		"http_redirect"
# define METHODS			"methods"
# define GET				"GET"
# define POST				"POST"
# define DELETE				"DELETE"
# define CGITITLE			"cgi"

// global functions
# define E_INVALIDBRACE		"Error: webserv: getInstruction: Invalid use of curly brace: "
# define E_INVALIDENDTOKEN	"Error: webserv: getInstruction: Missing end token ('{', '}' or ';'): "

// Server
# define E_S_CLIENTNOTFOUND	"Error: Server: Client fd not found in poll vector."
# define E_S_PSTRUCNOTFOUND	"Error: Server: Pollstruct fd not found in pollstruct vector."
# define E_S_NOBINDINGS		"Error: Server: No listening sockets bound."

# define I_S_BINDINGS		"Info: Server: " << _bindings.size() << (_bindings.size() == 1 ? " listening socket" : " listening sockets") << " successfully bound."

// close messages
# define CLOSE_POLLHUP		"POLLHUP."
# define CLOSE_NODATA		"No data received."
# define CLOSE_EXCPT		"CloseConnection exception caught."
# define CLOSE_DONE			"Done sending data."

// ConfigFile
# define E_CF_FILEOPEN		"Error: ServerConfig: Could not open config file: "
# define E_CF_ELMNTDECL		"Error: ConfigFile: Invalid element declaration, (only \"server\" allowed): "
# define E_CF_NOSERVER		"Error: ConfigFile: No valid Configs found."
# define E_CF_MANYSERVER	"Error: ConfigFile: Too many Configs found. Maximum of 10 allowed."
# define E_CF_SIZEMISMATCH	"Error: ConfigFile: Receive chunk size must be larger than max request head size."

# define I_CF_CONFIGIMPORT	"Info: ConfigFile: " << _configs.size() << " distinct" << (_configs.size() == 1 ? " Config" : " Configs") <<" extracted from " << configPath << "."

// Config
# define E_C_SUBELEMNT		"Error: Config: Second level subelements not allowed: "
# define E_C_INVALERRNUM	"Error: Config: Invalid HTML response code (range is from 100 to 599): "
# define E_C_HOSTADDRVAL	"Error: Config: Invalid address value. Cannot convert to IP address: "
# define E_C_PORTINPUT		"Error: Config: Invalid characters in port input. Only numerical allowed: "
# define E_C_PORTVAL		"Error: Config: Invalid port number. Must be from 0 to 65535: "
# define E_C_HOSTADDRVAL	"Error: Config: Invalid address value. Cannot convert to IP address: "
# define E_C_ROOTINPUT		"Error: Config: Invalid characters in root input: "
# define E_C_MAXCLBODINPUT	"Error: Config: Invalid characters in max client body input: "
# define E_C_MAXCLBODHIGH	"Error: Config: Invalid client max body size (too high): "
# define E_C_MAXCONNINPUT	"Error: Config: Invalid characters in max connections input. Only numerical allowed: "
# define E_C_MAXCONNVAL		"Error: Config: Invalid size of max connections: "
# define E_C_STDFILEINPUT	"Error: Config: Invalid characters in standard-file input: "

# define I_C_INVALIDVALUE	"Info: ServerConfig: Unrecognized or invalid value in config file: "
# define I_C_INVALIDKEY		"Info: ServerConfig: Unrecognized identifier in config file: "
# define I_C_MISSINGVAL		"Info: ServerConfig: Missing value for key: "
# define I_C_INVALSTATPAGE	"Info: ServerConfig: Skipping invalid status page key-value pair."
# define I_C_INVALSERVNAME	"Info: ServerConfig: Skipping invalid server name: "
# define I_C_NONDIRPATH		"Info: ServerConfig: Path does not end with a directory (no trailing '/'): "

// Server
# define E_ACCEPT			"Error: Server: accept()"
# define E_LISTEN			"Error: Server: listen()"
# define E_READ				"Error: Server: read()"
# define E_WRITE_CHPIPE		"Error: Server: write() (to child pipe)"
# define E_TEMPFILE			"Error: Server: Could not create temporary file."
# define E_POSTFILE			"Error: Server: Could not create / open POST file."
# define E_PIPE				"Error: Server: pipe()."
# define E_DUP2				"Error: Server: dup2()."
# define E_IFSTREAM			"Error: Server: ifstream object"

# define I_CONNECTIONLIMIT	"Info: Server: Connection limit reached."
# define I_CLOSENODATA		"Info: Server: Connection closed (no data received)."

// Client
# define E_CL_CHILD			"Error: Client: child exited with error or was terminated."
# define E_CL_CHILDTIMEOUT	"Error: Client: child timed out."
# define E_CL_TEMPFILEDEL	"Error: Client: Could not remove POST CGI tempfile."

// Response
# define E_SEND					"No data sent."
# define E_RECV					"No data received."
# define E_TEMPFILEREMOVAL		"Error: Response: Could not remove temp CGI-out file."

#endif

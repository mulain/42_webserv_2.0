# 42_webserv 2.0
A 42 School project about developing a C++98 web server.<br>
After submitting, I completely rewrote this project with a cleaner structure.

## Main points

- No threading allowed. Still have to serve multiple clients without blocking
- User must be able to configure multiple host:port combinations to listen on
- Multiple identical host:port combinations have to be resolved by host name
- Only one call to a multiplexing function is allowed (used poll())
- Manipulation of fds only allowed after going through the multiplexing function
- Implement basic CGI functionality
	- CGI must terminate after a time-out in case of infinite loop
	- Other clients should not be left hanging for the duration of the time-out
- Implement directory listing
- Implement http redirection
- Implement GET POST and DELETE methods
- Implement basic cookie

## Notes

- Response uses an abstract base class. Derived classes are for sending files or for sending (internally generated) dynamic content.
- Complex objects can create problems when stored directly in vectors. Better to allocate them manually and store the pointers in a vector. Cleaner and less work in the end.
- Control structure for CGI allows the main loop to continue while CGI request is being processed.

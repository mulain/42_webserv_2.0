# 42_webserv
A 42 School project about developing a C++98 web server<br>
<br>
Main points:
- No threading allowed. Still have to serve multiple clients without blocking
- User must be able to configure multiple host:port combinations to listen on
- Multiple identical host:port combinations have to be resolved by host name
- Only one call to a multiplexing function is allowed (I used poll())
- Manipulation of fds only allowed after going through the multiplexing function
- Implement basic CGI functionality
- Implement directory listing
- Implement http redirection
- Implement GET POST and DELETE methods
- Implement basic cookie

#ifndef TYPEDEFS_HPP
# define TYPEDEFS_HPP

typedef struct
{
	bool			get;
	bool			post;
	bool			delete_;
	std::string		dir_listing;
	std::string		http_redir;
	std::string		upload_dir;
	std::string		std_file;
}	s_locInfo;

typedef enum dynamicContentSelector
{
	dirListing,
	sessionLog
}	dynCont;

#endif

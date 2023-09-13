# include "webserv.hpp"

bool resourceExists(const std::string& path)
{
	if (path.empty())
		return false;
	
	std::ifstream	resource(path.c_str());
	bool			exists = resource.good();
	
	resource.close();
	return exists;
}

bool isDirectory(const std::string& path)
{
	struct stat	fileInfo;
	
	if (stat(path.c_str(), &fileInfo) == 0)
		return S_ISDIR(fileInfo.st_mode);
	return false;
}

size_t fileSize(const std::string& filePath)
{
	struct stat	fileInfo;

	if (filePath.empty())
		return 0;
	if (stat(filePath.c_str(), &fileInfo) != 0)
		return 0;
	return static_cast<size_t>(fileInfo.st_size);
}

std::string fileExtension(const std::string& filename)
{
	size_t dotPosition = filename.find_last_of(".");
	if (dotPosition == std::string::npos)
		return "";
	return filename.substr(dotPosition);
}

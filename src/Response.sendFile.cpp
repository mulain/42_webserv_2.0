#include "webserv.hpp"

SendFile::SendFile(int code, std::string sendPath, const Request& request):
	Response(request),
	_sendPath(sendPath),
	_responseHeadIncomplete(true),
	_filePosition(0)
{
	_code = code;
	_contentLength = fileSize(sendPath);
	_contentType = getMimeType(sendPath);
	_sendBuffer	<< buildResponseHead();
}

SendFile::SendFile(const SendFile& src):
	Response(src),
	_sendPath(src._sendPath),
	_responseHeadIncomplete(src._responseHeadIncomplete),
	_filePosition(src._filePosition)
{}

Response* SendFile::clone() const
{
	return new SendFile(*this);
}

bool SendFile::send(int fd)
{
	if (_responseHeadIncomplete)
	{
		whoIsI(fd);
		_responseHeadIncomplete = sendInternalBuffer(fd);
		return true;
	}

	std::ifstream	fileStream(_sendPath.c_str(), std::ios::binary);
	
	if (!fileStream)
	{
		fileStream.close();
		throw ErrorCode(500, __FUNCTION__);
	}
	
	char	buffer[SEND_CHUNK_SIZE];
	
	fileStream.seekg(_filePosition);
	fileStream.read(buffer, SEND_CHUNK_SIZE);

	if (::send(fd, buffer, fileStream.gcount(), 0) == -1)
	{
		fileStream.close();
		throw NetworkFailure(__FUNCTION__);
	}
	
	if (fileStream.eof())
	{
		fileStream.close();
		return false;
	}
	
	_filePosition = fileStream.tellg();
	fileStream.close();
	return true;
}

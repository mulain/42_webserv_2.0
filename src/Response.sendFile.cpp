#include "webserv.hpp"

SendFile::SendFile(std::string sendPath, const Request& request):
	Response(request),
	_sendPath(sendPath),
	_responseHeadIncomplete(true),
	_filePosition(0)
{
	ANNOUNCEME
	_code = 200;
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
	ANNOUNCEME
	if (_responseHeadIncomplete)
	{
		_responseHeadIncomplete = sendInternalBuffer(fd);
		printResponseHead(fd);
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

	int bytesSent = ::send(fd, buffer, fileStream.gcount(), 0);
	fileStream.close();
	
	if (bytesSent <= 0)
		throw NetworkFailure(__FUNCTION__);
	
	if (fileStream.eof())
		return false;
	
	_filePosition = fileStream.tellg();
	return true;
}

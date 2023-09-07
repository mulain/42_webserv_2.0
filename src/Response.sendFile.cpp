#include "../include/Response.sendFile.hpp"

SendFile::SendFile(std::string sendPath, const Request& request):
	Response(request),
	_sendPath(sendPath),
	_responseHeadSent(false),
	_filePosition(0)
{
	_code = 200;
	_contentLength = fileSize(sendPath);
	_contentType = getMimeType(sendPath);
	_sendBuffer	<< buildResponseHead();

}

SendFile::~SendFile()
{}

bool SendFile::send(int fd)
{
	char	buffer[SEND_CHUNK_SIZE];
	
	_sendBuffer.read(buffer, SEND_CHUNK_SIZE);
	
	if (::send(fd, buffer, _sendBuffer.gcount(), 0) <= 0)
		throw NetworkFailure(__FUNCTION__);
	
	if (_sendBuffer.tellg() == std::streampos(-1)) // end of buffer reached
		return false;
	return true;
}

bool SendFile::send(int fd)
{
	if (!_responseHeadSent)
		return (sendResponseHead(fd), true);

	std::ifstream fileStream(_sendPath.c_str(), std::ios::binary);
	if (!fileStream)
	{
		fileStream.close();
		throw ErrorCode(500, __FUNCTION__);
	}
	
	char	buffer[SEND_CHUNK_SIZE];
	
	fileStream.seekg(_filePosition);
	fileStream.read(buffer, SEND_CHUNK_SIZE);

	if (::send(fd, buffer, fileStream.gcount(), 0) <= 0)
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

void SendFile::sendResponseHead(int fd)
{
	char	buffer[SEND_CHUNK_SIZE];

	_sendBuffer.read(buffer, SEND_CHUNK_SIZE);
	if (::send(fd, buffer, _sendBuffer.gcount(), 0) <= 0) // this assumes sendchunk big enough for entire response
		throw NetworkFailure(__FUNCTION__);
	_responseHeadSent = true;
}

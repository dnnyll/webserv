#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"

void	HttpRequest::decodeBody()
{

	if (contentLength > _maxBodySize)
	{
		_state = ERROR_STATE;
		_errorReason = BODY_TOO_LARGE;
		return ;
	}

	size_t	bytesNeeded = contentLength - _bodyBytesRead;
	size_t	bytesAvailable = _buffer.size();
	size_t	toRead;

	if (bytesAvailable < bytesNeeded)
		toRead = bytesAvailable;
	else
		toRead = bytesNeeded;

	body += _buffer.substr(0, toRead);
	_bodyBytesRead += toRead;

	if (_bodyBytesRead > _maxBodySize)
	{
		_state = ERROR_STATE;
		_errorReason = BODY_TOO_LARGE;
		return ;
	}

	_buffer.erase(0, toRead);

	if (_bodyBytesRead >= contentLength)
		_state = COMPLETE;
}

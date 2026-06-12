#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"

/*
	decodeBody() flow

	_buffer contains raw body bytes — headers are already gone.
	It calculates how many bytes are still missing (contentLength - _bodyBytesRead),
	takes only that many from _buffer,
	appends them to body. 
	Any leftover bytes stay in _buffer.
	This repeats across multiple chunks until _bodyBytesRead == contentLength,
	at which point the full body is assembled and _state becomes COMPLETE.
*/
void	HttpRequest::decodeBody()
{
	std::cout << "=====\tdecodeBody()" << std::endl;

	debugParse("BODY", "buffer size", _buffer.size());
	debugParse("BODY", "contentLength", contentLength);
	debugParse("BODY", "already read", _bodyBytesRead);

	size_t	bytesNeeded = contentLength - _bodyBytesRead;
	size_t	bytesAvailable = _buffer.size();

	debugParse("BODY", "bytesNeeded", bytesNeeded);
	debugParse("BODY", "bytesAvailable", bytesAvailable);

	size_t	toRead;

	if (bytesAvailable < bytesNeeded)
		toRead = bytesAvailable;
	else
		toRead = bytesNeeded;

	debugParse("BODY", "toRead", toRead);
	debugParse("BODY", "buffer", _buffer);

	body += _buffer.substr(0, toRead);
	_bodyBytesRead += toRead;

	debugParse("BODY", "body", body);

	_buffer.erase(0, toRead);

	debugParse("BODY", "buffer after erase", _buffer.size());

	if (_bodyBytesRead >= contentLength)
	{
		_state = COMPLETE;
		debugParse("BODY", "state", "COMPLETE");
	}
}

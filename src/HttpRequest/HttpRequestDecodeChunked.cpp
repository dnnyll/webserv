#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<string>
#include	<sstream>

bool	HttpRequest::parseChunkSize()
{
		size_t	pos = _buffer.find("\r\n");
		debugParse("CHUNKED", "CRLF position", pos);

		if (pos == std::string::npos)
		{
			debugParse("CHUNKED", "status", "incomplete size line");
			return (false);
		}

		std::string	sizeLine = _buffer.substr(0, pos);

		debugParse("CHUNKED", "size line", sizeLine);

		_buffer.erase(0, pos + 2);

		std::stringstream	ss(sizeLine);
		ss >> std::hex >> _chunkSize;

		debugParse("CHUNKED", "parsed chunkSize", _chunkSize);

		return (true);
}

void	HttpRequest::decodeChunked()
{
	std::cout << "=====\tdecodeChunked()" << std::endl;

	debugParse("CHUNKED", "buffer size", _buffer.size());
	debugParse("CHUNKED", "chunkSize", _chunkSize);

	if (_chunkSize == 0)
	{
		if (_chunkSize == 0)
		{
			if (!parseChunkSize())
				return ;
			if (_chunkSize == 0)
			{
				debugParse("CHUNKED", "status", "final chunk received");
				_state = COMPLETE;
				return ;
			}
		}
	}

	while (1)
	{
		debugParse("CHUNKED", "buffer size", _buffer.size());
		debugParse("CHUNKED", "chunkSize", _chunkSize);

		if (_buffer.size() < _chunkSize + 2)
		{
			debugParse("CHUNKED", "status", "incomplete chunk data");
			return ;
		}

		body += _buffer.substr(0, _chunkSize);
		debugParse("CHUNKED", "body so far", body);

		_buffer.erase(0, _chunkSize + 2);
		debugParse("CHUNKED", "remaining buffer size", _buffer.size());

		_chunkSize = 0;

		if (!parseChunkSize())
			return ;
		if (_chunkSize == 0)
		{
			debugParse("CHUNKED", "status", "final chunk received");
			_state = COMPLETE;
			return ;
		}
	}
}

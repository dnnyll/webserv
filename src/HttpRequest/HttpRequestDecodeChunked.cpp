#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<string>
#include	<sstream>

bool	HttpRequest::parseChunkSize()
{
		size_t	pos = _buffer.find("\r\n");

		if (pos == std::string::npos)
			return (false);

		std::string	sizeLine = _buffer.substr(0, pos);

		_buffer.erase(0, pos + 2);

		std::stringstream	ss(sizeLine);
		ss >> std::hex >> _chunkSize;

		return (true);
}

void	HttpRequest::decodeChunked()
{
	if (_chunkSize == 0)
	{
		if (!parseChunkSize())
			return ;
		if (_chunkSize == 0)
		{
			_state = COMPLETE;
			return ;
		}
	}

	while (1)
	{
		if (_buffer.size() < _chunkSize + 2)
			return ;

		body += _buffer.substr(0, _chunkSize);

		if (body.size() > _maxBodySize)
		{
			_state = ERROR_STATE;
			_errorReason = BODY_TOO_LARGE;
			return ;
		}

		_buffer.erase(0, _chunkSize + 2);
		_chunkSize = 0;

		if (!parseChunkSize())
			return ;

		if (_chunkSize == 0)
		{
			_state = COMPLETE;
			return ;
		}
	}
}

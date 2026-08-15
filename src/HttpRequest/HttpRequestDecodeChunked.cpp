#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<string>
#include	<sstream>

bool	HttpRequest::parseChunkSize()
{
	size_t	crlf_pos = _buffer.find("\r\n");
	size_t	lf_pos = _buffer.find('\n');

	if (lf_pos != std::string::npos
		&& (crlf_pos == std::string::npos || lf_pos < crlf_pos))
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	if (crlf_pos == std::string::npos)
		return (false);

	std::string sizeLine = _buffer.substr(0, crlf_pos);

	// strip chunk extensions (";foo=bar") before parsing size
	size_t	ext_pos = sizeLine.find(';');

	if (ext_pos != std::string::npos)
		sizeLine = sizeLine.substr(0, ext_pos);

	if (sizeLine.empty())
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	std::stringstream ss(sizeLine);
	ss >> std::hex >> _chunkSize;

	if (ss.fail() || !ss.eof())
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	_buffer.erase(0, crlf_pos + 2);
	return (true);
}

void	HttpRequest::decodeChunked()
{
	if (!parseChunkSize())
		return ;

	if (_chunkSize == 0)
	{
		_state = COMPLETE;
		return ;
	}
	
	if (_chunkSize > _maxBodySize || body.size() + _chunkSize > _maxBodySize)
	{
		_state = ERROR_STATE;
		_errorReason = BODY_TOO_LARGE;
		return ;
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

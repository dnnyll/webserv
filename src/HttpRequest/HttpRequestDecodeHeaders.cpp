#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<sstream>
#include	<string>

bool    HttpRequest::extractHeaderLine(std::string &line, size_t &pos)
{
	size_t crlf_pos = _buffer.find("\r\n");
	size_t lf_pos = _buffer.find('\n');

	if (lf_pos != std::string::npos
		&& (crlf_pos == std::string::npos || lf_pos < crlf_pos))
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	pos = crlf_pos;

	if (pos == std::string::npos)
		return (false);

	if (pos == 0)
		return (true);

	line = _buffer.substr(0, pos);

	return (true);
}

bool	HttpRequest::splitHeaderLine(const std::string &line)
{
	size_t	separator = line.find(": ");

	if (separator == std::string::npos)
	{
		_state = ERROR_STATE;
		return (false);
	}

	std::string	key = line.substr(0, separator);
	std::string	value = line.substr(separator + 2);
	headers[key] = value;

	return (true);
}

void	HttpRequest::resolveBodyState()
{
	bool	hasTE = headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked";
	bool	hasCL = headers.count("Content-Length");

	if (hasTE && hasCL)
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return ;
	}
	if (hasTE)
	{
		isChunked = true;
		_state = CHUNKED;
	}
	else if (hasCL)
	{
		std::istringstream ss(headers["Content-Length"]);
		ss >> contentLength;

		if (contentLength > _maxBodySize)
		{
			_state = ERROR_STATE;
			_errorReason = BODY_TOO_LARGE;
			return ;
		}
		_state = BODY;
	}
	else
		_state = COMPLETE;
}

void	HttpRequest::decodeHeaders()
{
	while (1)
	{
		std::string	line;
		size_t		pos;

		if (!extractHeaderLine(line, pos))
			return ;

		if (pos == 0)
		{
			_buffer.erase(0, 2);
			resolveBodyState();
			return ;
		}

		if (!splitHeaderLine(line))
			return ;

		_buffer.erase(0, pos + 2);
	}
}

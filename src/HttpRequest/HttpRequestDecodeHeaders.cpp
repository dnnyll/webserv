#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<sstream>
#include	<string>

bool	HttpRequest::extractHeaderLine(std::string &line, size_t &pos)
{
	pos = _buffer.find("\r\n");

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
	if (headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked")
	{
		isChunked = true;
		_state = CHUNKED;
	}
	else if (headers.count("Content-Length"))
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

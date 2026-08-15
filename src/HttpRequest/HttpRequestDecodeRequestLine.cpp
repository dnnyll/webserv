#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<string>

// CRLF = Carriage Return + Line Feed
// LF = Line Feed only
bool	HttpRequest::extractRequestLine(std::string &line)
{
	size_t	crlf_pos = _buffer.find("\r\n");
	size_t	lf_pos = _buffer.find('\n');

	if (lf_pos != std::string::npos && (crlf_pos == std::string::npos || lf_pos < crlf_pos))
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	if (crlf_pos == std::string::npos)
		return (false);

	line = _buffer.substr(0, crlf_pos);
	_buffer.erase(0, crlf_pos + 2);

	return (true);
}

bool	HttpRequest::splitRequestLine(const std::string &line)
{
	size_t	first_space = line.find(' ');

	if (first_space == std::string::npos)
		{
			_state = ERROR_STATE;
			return (false);
		}

	size_t	second_space = line.find(' ', first_space + 1);

	if (second_space == std::string::npos)
	{
		_state = ERROR_STATE;
		return (false);
	}

	method = line.substr(0, first_space);
	uri = line.substr(first_space + 1, second_space - first_space - 1);
	version = line.substr(second_space + 1);

	return (true);
}

bool	HttpRequest::validateRequestLine()
{
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		_state = ERROR_STATE;
		_errorReason = METHOD_NOT_ALLOWED;
		return (false);
	}

	if (uri.empty() || uri[0] != '/')
	{
		_state = ERROR_STATE;
		return (false);
	}
	if (version != "HTTP/1.1")
	{
		_state = ERROR_STATE;
		return (false);
	}

	return (true);
}

void	HttpRequest::decodeRequestLine()
{
	std::string	line;

	while (_buffer.size() >= 2 && _buffer[0] == '\r' && _buffer[1] == '\n')
		_buffer.erase(0, 2);

	if (!extractRequestLine(line))
		return ;

	if (!splitRequestLine(line))
		return ;

	if (!validateRequestLine())
		return ;

	_state = HEADERS;
}

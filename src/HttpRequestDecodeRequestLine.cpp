#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"

bool	HttpRequest::extractRequestLine(std::string &line)
{
	size_t pos = _buffer.find("\r\n");
	if (pos == std::string::npos)
		return false;
	line = _buffer.substr(0, pos);
	_buffer.erase(0, pos + 2);
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


void HttpRequest::decodeRequestLine()
{
	std::string	line;

	if (!extractRequestLine(line))
	{
		debugParse("REQUEST_LINE", "status", "incomplete");
		return ;
	}
	debugParse("REQUEST_LINE", "line", line);

	if (!splitRequestLine(line))
	{
		debugParse("REQUEST_LINE", "ERROR", "split failed");
		return ;
	}
	debugParse("REQUEST_LINE", "method", method);
	debugParse("REQUEST_LINE", "uri", uri);
	debugParse("REQUEST_LINE", "version", version);

	if (!validateRequestLine())
	{
		debugParse("REQUEST_LINE", "ERROR", "validation failed");
		return ;
	}
	_state = HEADERS;
	debugParse("REQUEST_LINE", "state", "HEADERS");
}

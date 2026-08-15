#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<cctype>
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
	size_t	colon = line.find(':');

	if (colon == std::string::npos)
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	std::string	key = line.substr(0, colon);
	std::string	value = line.substr(colon + 1);

	//	RFC 7230 §3.2.4: no whitespace allowed between field-name and colon
	if (key.empty() || key.find_first_of(" \t") != std::string::npos)
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

	size_t	first = value.find_first_not_of(" \t");
	size_t	last = value.find_last_not_of(" \t");

	if (first == std::string::npos)
		value.clear();
	else
		value = value.substr(first, last - first + 1);

	//	duplicate field name -> reject (Host, Content-Length, ... must be single)
	if (headers.count(key) > 0)
	{
		_state = ERROR_STATE;
		_errorReason = MALFORMED_REQUEST;
		return (false);
	}

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
		std::string	cl = headers["Content-Length"];
		size_t		first = cl.find_first_not_of(" \t");
		size_t		last = cl.find_last_not_of(" \t");

		if (first == std::string::npos)
		{
			_state = ERROR_STATE;
			_errorReason = MALFORMED_REQUEST;
			return ;
		}
		cl = cl.substr(first, last - first + 1);

		for (size_t i = 0; i < cl.size(); ++i)
		{
			if (!std::isdigit(static_cast<unsigned char>(cl[i])))
			{
				_state = ERROR_STATE;
				_errorReason = MALFORMED_REQUEST;
				return ;
			}
		}

		std::istringstream ss(cl);
		ss >> contentLength;

		if (ss.fail() || contentLength > _maxBodySize)
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

			//	RFC 7230 §5.4: Host is mandatory in HTTP/1.1
			if (!headers.count("Host"))
			{
				_state = ERROR_STATE;
				_errorReason = MALFORMED_REQUEST;
				return ;
			}

			resolveBodyState();
			return ;
		}

		if (!splitHeaderLine(line))
			return ;

		_buffer.erase(0, pos + 2);
	}
}

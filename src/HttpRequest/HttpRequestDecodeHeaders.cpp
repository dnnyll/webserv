#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"
#include	<sstream>
#include	<string>

/*
	Note on find function: if it doesn't find what it's looking for it
	will return npos, thus checking for npos multiple times in case
	we haven't found what we are looking for.
*/

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

		//	insert here if (bodyLength > _maxBodySize)
		//	_state = ERROR_STATE
		
		_state = BODY;
	}
	else
		_state = COMPLETE;
}

void	HttpRequest::decodeHeaders()
{
	std::cout << "=====\tdecodeHeaders()" << std::endl;
	debugParse("HEADERS", "buffer size", _buffer.size());
	
	while (1)
	{
		std::string	line;
		size_t		pos;

		if (!extractHeaderLine(line, pos))
		{
			debugParse("HEADERS", "status", "incomplete header line");
			return ;
		}

		if (pos == 0)
		{
			debugParse("HEADERS", "status", "end of headers found");
			_buffer.erase(0, 2);
			resolveBodyState();
			debugParse("HEADERS", "state", _state);
			return ;
		}

		debugParse("HEADERS", "header line", line);

		if (!splitHeaderLine(line))
		{
			debugParse("HEADERS", "ERROR", "invalid header format");		
			return ;
		}

		debugParse("HEADERS", "stored header", line);
		_buffer.erase(0, pos + 2);
		debugParse("HEADERS", "remaining buffer size", _buffer.size());
	}
}


// decodeBody();

// decodeChunked();

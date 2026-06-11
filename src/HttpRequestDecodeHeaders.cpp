#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"

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
	size_t separator = line.find(": ");
	if (separator == std::string::npos)
		{ _state = ERROR_STATE; return false; }
	std::string key   = line.substr(0, separator);
	std::string value = line.substr(separator + 2);
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
		_state = BODY;
	}
	else
		_state = COMPLETE;
}

void HttpRequest::decodeHeaders()
{
	debugParse("HEADERS", "buffer size", _buffer.size());
	while (1)
	{
		std::string line;
		size_t      pos;

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




















// void	HttpRequest::decodeHeaders()
// {
// 	debugParse("HEADERS", "header buffer size", _buffer.size());

// 	while (1)
// 	{
// 		size_t	pos = _buffer.find("\r\n");
// 		debugParse("HEADERS", "CRLF position", pos);

// 		if (pos == std::string::npos)
// 		{
// 			debugParse("HEADERS", "status", "incomplete header line");
// 			return;
// 		}

// 		if (pos == 0)
// 		{
// 			debugParse("HEADERS", "status", "end of headers found");

// 			_buffer.erase(0, 2);

// 			_state = BODY;
// 			debugParse("HEADERS", "state", "BODY");

// 			return;
// 		}

// 		std::string line = _buffer.substr(0, pos);
// 		debugParse("HEADERS", "header line", line);

// 		size_t separator = line.find(": ");
// 		debugParse("HEADERS", "separator position", separator);

// 		if (separator == std::string::npos)
// 		{
// 			debugParse("HEADERS", "ERROR", "invalid header format");

// 			_state = ERROR_STATE;
// 			return;
// 		}

// 		std::string key = line.substr(0, separator);
// 		std::string value = line.substr(separator + 2);

// 		debugParse("HEADERS", "header key", key);
// 		debugParse("HEADERS", "header value", value);

// 		headers[key] = value;

// 		debugParse("HEADERS", "stored header", key);

// 		_buffer.erase(0, pos + 2);

// 		debugParse("HEADERS", "remaining buffer size", _buffer.size());
// 	}
// 	//check	if body
// 	if (headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked")
// 	{
// 		isChunked = true;
// 		_state = CHUNKED;
// 	}
// 	else if (headers.count("Content-Length"))
// 	{
// 		contentLength = /* convert headers["Content-Length"] from string to size_t */
// 		_state = BODY;
// 	}
// 	else
// 		_state = COMPLETE; // no body
// 	}



// decodeBody();

// decodeChunked();
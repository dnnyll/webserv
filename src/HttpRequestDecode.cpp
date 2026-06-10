#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"

bool	HttpRequest::extractRequestLine(std::string &line)
{
	size_t pos = _buffer.find("\r\n");
	if (pos == std::string::npos)
		return false;
	line = _buffer.substr(0, pos);
	_buffer.erase(0, pos + 2);
	return true;
}

bool	HttpRequest::splitRequestLine(const std::string &line)
{
	size_t	first_space = line.find(' ');

	if (first_space == std::string::npos)
		{ _state = ERROR_STATE; return false; }

	size_t second_space = line.find(' ', first_space + 1);

	if (second_space == std::string::npos)
		{ _state = ERROR_STATE; return false; }

	method = line.substr(0, first_space);
	uri = line.substr(first_space + 1, second_space - first_space - 1);
	version = line.substr(second_space + 1);
	return (true);
}

bool	HttpRequest::validateRequestLine()
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		{ _state = ERROR_STATE; return false; }

	if (uri.empty() || uri[0] != '/')
		{ _state = ERROR_STATE; return false; }

	if (version != "HTTP/1.1")
		{ _state = ERROR_STATE; return false; }

	return true;
}


void HttpRequest::decodeRequestLine()
{
	std::string line;

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













// //	we wait for the full line
// //	we extract the line
// //	we split by spaces
// //	we validate (GET, POST, DELETE)
// //	we consume data from buffer
// //	we advance, set _state = HEADERS
// void	HttpRequest::decodeRequestLine()
// {
// 	size_t	pos = _buffer.find("\r\n");
// 	debugParse("REQUEST_LINE", "\r\n", pos);

// 	if (pos == std::string::npos)
// 	{
// 		debugParse("REQUEST_LINE", "status", "incomplete");
// 		return;
// 	}

// 	std::string	line = _buffer.substr(0, pos);
// 	debugParse("REQUEST_LINE", "line", line);

// 	size_t	first_space = line.find(' ');
// 	debugParse("REQUEST_LINE", "first_space", first_space);

// 	if (first_space == std::string::npos)
// 	{
// 		debugParse("REQUEST_LINE", "ERROR", "first space not found");
// 		_state = ERROR_STATE;
// 		return;
// 	}

// 	size_t	second_space = line.find(' ', first_space + 1);
// 	debugParse("REQUEST_LINE", "second_space", second_space);

// 	if (second_space == std::string::npos)
// 	{
// 		debugParse("REQUEST_LINE", "ERROR", "second space not found");
// 		_state = ERROR_STATE;
// 		return;
// 	}

// 	method = line.substr(0, first_space);
// 	uri = line.substr(first_space + 1,
// 					  second_space - first_space - 1);
// 	version = line.substr(second_space + 1);

// 	debugParse("REQUEST_LINE", "method", method);
// 	debugParse("REQUEST_LINE", "uri", uri);
// 	debugParse("REQUEST_LINE", "version", version);

// 	if (method != "GET" && method != "POST" && method != "DELETE")
// 	{
// 		debugParse("REQUEST_LINE", "ERROR", "invalid method");
// 		_state = ERROR_STATE;
// 		return;
// 	}

// 	if (uri.empty() || uri[0] != '/')
// 	{
// 		debugParse("REQUEST_LINE", "ERROR", "invalid URI");
// 		_state = ERROR_STATE;
// 		return;
// 	}

// 	if (version != "HTTP/1.1")
// 	{
// 		debugParse("REQUEST_LINE", "ERROR", "invalid HTTP version");
// 		_state = ERROR_STATE;
// 		return;
// 	}

// 	debugParse("REQUEST_LINE", "status", "request line valid");

// 	_buffer.erase(0, pos + 2);

// 	_state = HEADERS;
// 	debugParse("REQUEST_LINE", "state", "HEADERS");
// }

void	HttpRequest::decodeHeaders()
{
	debugParse("HEADERS", "header buffer size", _buffer.size());

	while (1)
	{
		size_t pos = _buffer.find("\r\n");
		debugParse("HEADERS", "CRLF position", pos);

		if (pos == std::string::npos)
		{
			debugParse("HEADERS", "status", "incomplete header line");
			return;
		}

		if (pos == 0)
		{
			debugParse("HEADERS", "status", "end of headers found");

			_buffer.erase(0, 2);

			_state = BODY;
			debugParse("HEADERS", "state", "BODY");

			return;
		}

		std::string line = _buffer.substr(0, pos);
		debugParse("HEADERS", "header line", line);

		size_t separator = line.find(": ");
		debugParse("HEADERS", "separator position", separator);

		if (separator == std::string::npos)
		{
			debugParse("HEADERS", "ERROR", "invalid header format");

			_state = ERROR_STATE;
			return;
		}

		std::string key = line.substr(0, separator);
		std::string value = line.substr(separator + 2);

		debugParse("HEADERS", "header key", key);
		debugParse("HEADERS", "header value", value);

		headers[key] = value;

		debugParse("HEADERS", "stored header", key);

		_buffer.erase(0, pos + 2);

		debugParse("HEADERS", "remaining buffer size", _buffer.size());
	}
	//check	if body
	if (headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked")
	{
		isChunked = true;
		_state = CHUNKED;
	}
	// else if (headers.count("Content-Length"))
	// {
	// 	contentLength = /* convert headers["Content-Length"] from string to size_t */
	// 	_state = BODY;
	// }
	else
		_state = COMPLETE; // no body
	}



// decodeBody();

// decodeChunked();
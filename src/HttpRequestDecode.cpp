#include	"../inc/HttpRequest.hpp"
#include	"../inc/HttpRequestDecodeDebug.hpp"

//	we wait for the full line
//	we extract the line
//	we split by spaces
//	we validate (GET, POST, DELETE)
//	we consume data from buffer
//	we advance, set _state = HEADERS
void HttpRequest::decodeRequestLine()
{
	size_t pos = _buffer.find("\r\n");
	debugRequestLine("CRLF position", pos);

	if (pos == std::string::npos)
	{
		debugRequestLine("status", "no complete request line yet");
		return;
	}

	std::string line = _buffer.substr(0, pos);
	debugRequestLine("line", line);

	size_t first_space = line.find(' ');
	debugRequestLine("first_space", first_space);

	if (first_space == std::string::npos)
	{
		debugRequestLine("ERROR", "first space not found");
		_state = ERROR_STATE;
		return;
	}

	size_t second_space = line.find(' ', first_space + 1);
	debugRequestLine("second_space", second_space);

	if (second_space == std::string::npos)
	{
		debugRequestLine("ERROR", "second space not found");
		_state = ERROR_STATE;
		return;
	}

	method = line.substr(0, first_space);
	uri = line.substr(first_space + 1,
					  second_space - first_space - 1);
	version = line.substr(second_space + 1);

	debugRequestLine("method", method);
	debugRequestLine("uri", uri);
	debugRequestLine("version", version);

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		debugRequestLine("ERROR", "invalid method");
		_state = ERROR_STATE;
		return;
	}

	if (uri.empty() || uri[0] != '/')
	{
		debugRequestLine("ERROR", "invalid URI");
		_state = ERROR_STATE;
		return;
	}

	if (version != "HTTP/1.1")
	{
		debugRequestLine("ERROR", "invalid HTTP version");
		_state = ERROR_STATE;
		return;
	}

	debugRequestLine("status", "request line valid");

	_buffer.erase(0, pos + 2);

	_state = HEADERS;
	debugRequestLine("state", "HEADERS");
}

// void	HttpRequest::decodeRequestLine()
// {
// 	size_t	pos = _buffer.find("\r\n");

// 	if (pos == std::string::npos)
// 		return ;

// 	std::string	line = _buffer.substr(0, pos);

// 	size_t	first_space = line.find(' ');

// 	if (first_space == std::string::npos)
// 	{
// 		_state = ERROR_STATE;
// 		return;
// 	}

// 	size_t	second_space = line.find(' ', first_space + 1);
 
// 	if (second_space == std::string::npos)
// 	{
// 		_state = ERROR_STATE;
// 		return ;
// 	}

// 	method = line.substr(0, first_space);
// 	uri  = line.substr(first_space + 1, second_space - first_space - 1);
// 	version = line.substr(second_space + 1);

// 	if (method != "GET" && method != "POST" && method != "DELETE")
// 	{
// 		_state = ERROR_STATE;
// 		return ;
// 	}
	
// 	if (uri.empty() || uri[0] != '/')
// 	{
// 		_state = ERROR_STATE;
// 		return ;
// 	}

// 	if (version != "HTTP/1.1")
// 	{
// 		_state = ERROR_STATE;
// 		return ;
// 	}

// 	_buffer.erase(0, pos + 2); // +2 to skip \r\n

// 	_state = HEADERS;
// }

void	HttpRequest::decodeHeaders()
{

}

// decodeBody();

// decodeChunked();
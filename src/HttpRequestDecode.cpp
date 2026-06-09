#include	"../inc/HttpRequest.hpp"

//	we wait for the full line
//	we extract the line
//	we split by spaces
//	we validate (GET, POST, DELETE)
//	we consume data from buffer
//	we advance, set _state = HEADERS

void	HttpRequest::decodeRequestLine()
{
	size_t	pos = _buffer.find("\r\n");

	if (pos == std::string::npos)
		return ;

	std::string	line = _buffer.substr(0, pos);

	size_t	first_space = line.find(' ');

	if (first_space == std::string::npos)
		{ _state = ERROR_STATE; return; }

	size_t	second_space = line.find(' ', first_space + 1);

	if (second_space == std::string::npos)
		{ _state = ERROR_STATE; return; }

	method = line.substr(0, first_space);
	uri  = line.substr(first_space + 1, second_space - first_space - 1);
	version = line.substr(second_space + 1);

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		_state = ERROR_STATE;
		return ;
	}
	
	if (uri.empty() || uri[0] != '/')
	{
		_state = ERROR_STATE;
		return ;
	}

	if (version != "HTTP/1.1")
	{
		_state = ERROR_STATE;
		return ;
	}

	_buffer.erase(0, pos + 2); // +2 to skip \r\n

	_state = HEADERS;
}

void	HttpRequest::decodeHeaders()
{
	
}

// decodeBody();

// decodeChunked();
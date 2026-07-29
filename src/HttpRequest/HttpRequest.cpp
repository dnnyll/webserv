#include	"HttpRequest.hpp"

// TODO (danny): check maxBodySize a prendre de la config
HttpRequest::HttpRequest()
	: contentLength(0)
	, isChunked(false)
	, _chunkSize(0)
	, _state(REQUEST_LINE)
	, _bodyBytesRead(0)
	, _maxBodySize(1000000)
{
	(void)_bodyBytesRead;
}

//	sets maxbodysize from Config
void	HttpRequest::setMaxBodySize(size_t size)
{
	_maxBodySize = size;
}
//	append incoming bytes to internal buffer
//	the buffer may already contain leftover bytes from last call
bool	HttpRequest::getData(const std::string& chunk)
{
	_buffer += chunk;
	while (_state != COMPLETE && _state != ERROR_STATE)
	{
		ParseState	prev = _state;
		decode();
		if (_state == prev)
			break ;
	}

	std::cout << "final state: " << _state << std::endl;

	return (_state == COMPLETE);
}

bool	HttpRequest::decode()
{
	switch(_state)
	{
		case REQUEST_LINE:
			decodeRequestLine();
			std::cout << "REQUEST_LINE" << std::endl;
			break ;
		case HEADERS:
			decodeHeaders();
			std::cout << "HEADERS" << std::endl;
			break ;
		case BODY:
			decodeBody();
			std::cout << "BODY" << std::endl;
			break ;
		case CHUNKED:
			decodeChunked();
			std::cout << "CHUNKED" << std::endl;
			break ;
		case COMPLETE:
			std::cout << "COMPLETE" << std::endl;
			break ;
		case ERROR_STATE:
			std::cout << "ERROR" << std::endl;
			break ;
	}
	return(_state == COMPLETE);

}

void	HttpRequest::reset()
{
	method.clear();
	uri.clear();
	version.clear();
	headers.clear();
	body.clear();

	contentLength = 0;
	isChunked = false;

	_state = REQUEST_LINE;
	_bodyBytesRead = 0;
	_chunkSize = 0;
}

bool	HttpRequest::isComplete() const
{
	return (_state == COMPLETE);
}

bool	HttpRequest::hasError() const
{
	return (_state == ERROR_STATE);
}


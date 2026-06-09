/*
	This part is responsible for taking raw incoming data from the client,
	gradually turning it into a structured HTTP request. 
	It keeps reading chunks until the full request (headers + body) is complete.
	In the end, it tells the system “I have everything, you can process this request now.”
*/

#include	"HttpRequest.hpp"

//	constructor ──────────────────────────────────────────────────────

HttpRequest::HttpRequest()
	: contentLength(0)
	, isChunked(false)
	, _state(REQUEST_LINE)
	, _bodyBytesRead(0)
{
	(void)_bodyBytesRead;
	//	parser starts at REQUEST_LINE
	//	no body length known yet
	//	no chunked encoding assumed ye
}


//	feed ─────────────────────────────────────────────────────────────

//	append incoming bytes to internal buffer
//	the buffer may already contain leftover bytes from last call
//	real parser will be added here later (check doc webserv_httpReq_feed.txt)

bool HttpRequest::getData(const std::string& chunk)
{
	_buffer += chunk;
	while (_state != COMPLETE && _state != ERROR_STATE)
	{
		ParseState	prev = _state;
		decode();
		if (_state == prev)
			break ;
	}
	//	debug
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
			// decodeBody();
			std::cout << "BODY" << std::endl;
			break ;
		case CHUNKED:
			// decodeChunked();
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

//	isComplete ───────────────────────────────────────────────────────

bool	HttpRequest::isComplete() const
{
	//	only true when state machine reached COMPLETE
	//	ClientHandler calls this after every feed()
	//	when true: ClientHandler calls Router::route()
	return (_state == COMPLETE);
}


//	hasError ─────────────────────────────────────────────────────────
bool	HttpRequest::hasError() const
{
	//	only true when state machine reached ERROR_STATE
	//	ClientHandler checks this after every feed()
	//	when true: ClientHandler builds 400 or 505 response directly
	//	without calling Router::route()
	return (_state == ERROR_STATE);
}


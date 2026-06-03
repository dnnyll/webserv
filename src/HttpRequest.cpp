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

bool	HttpRequest::getData(const std::string& chunk)
{
	//	append incoming bytes to internal buffer
	//	the buffer may already contain leftover bytes from last call
	//	real parser will be added here later (check doc webserv_httpReq_feed.txt)
	_buffer += chunk;
	return (false);
	//stub: real implementation returns (_state == COMPLETE)
}

bool	HttpRequest::decode()
{
	switch(_state)
	{
		case REQUEST_LINE:
			std::cout << "decodeRequestLine()" << std::endl;
			
		case HEADERS:
			std::cout << "decodeHeader()" << std::endl;
			
		case BODY:
			std::cout << "decodeBody()" << std::endl;
			
		case CHUNKED:
			std::cout << "decodeChunk()" << std::endl;

		case COMPLETE:
			std::cout << "COMPLETE" << std::endl;

		case ERROR_STATE:
			std::cout << "ERROR" << std::endl;
	}
	return(_state = COMPLETE);

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


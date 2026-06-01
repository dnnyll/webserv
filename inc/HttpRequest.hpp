#ifndef		HTTPREQUEST_HPP
# define	HTTPREQUEST_HPP

#include	<string>
#include	<map>

enum	ParseState
{
	REQUEST_LINE,		//	parsing: "GET /index.html/1.1"
	HEADERS,			//	is parsing HTTP headers
	BODY,				//	is parsing request body
	CHUNKED,			//	parsing chunked transfer-encoding body
	COMPLETE,			//	is request fully parsed
	ERROR_STATE			//	malformed request
};

class	HttpRequest
{
	public:
		//	request line fields ──────────────────────────────────

		std::string	method;
		//	http Method: GET POST DELETE

		std::string	uri;
		//	request URI/path
		//	"/images/cat.png"

		std::string	version;
		//	to verify that it's "HTTP/1.1" — anything else → ERROR_STATE 505

		
		//	header fields ────────────────────────────────────────

		std::map<std::string, std::string> headers;
		//	all parsed headers stored here
		//	headers["Host"]           = "localhost:8080"
		//	headers["Content-Length"] = "42"
		//	headers["Connection"]     = "keep-alive"
		//	headers["Transfer-Encoding"] = "chunked"
		//	HTTP/1.1: Host header is REQUIRED — missing → 400


		//	body field ───────────────────────────────────────────

		std::string	body;
		//	populated after BODY or CHUNKED state completes
		//	used for POST uploads, CGI stdin
		//	used for POST uploads/images


		//	── parsed metadata ──────────────────────────────────────

		size_t	contentLength;
		//	content length extracted from Headers
		//	value of contentLength header converted to size_t
		//	0 if header not present or not applicable

		bool	isChunked;
		//	true when Transfer-Encoding: chunked is set
		//	when true: contentLength is ignored, use chunked parser

	public:
		HttpRequest();

		bool	feed(const std::string& chunk);
		//	append new raw bytes from recv() to internal buffer
		//	advances the parse state machine
		//	returns true if request reaches COMPLETE state
		//	safe to call multiple times with partial data
		//	data arrives in chunks — never assume full request in one call
		
		bool	isComplete() const;
		//	returns true only when state == COMPLETE
		//	ClientHandler checks this after every feed() call

		bool	hasError() const;
		//	returns true when state == ERROR_STATE
		//	ClientHandler checks this to decide 400 or 505 response

		private:
			ParseState	_state;
			//	current position in the state machine
			//	only feed() is allowed to advance this
			//	public code reads state via isComplete() / hasError() only

		std::string	_buffer;
			//	raw bytes waiting to be parsed
			//	feed() appends incoming chunks here
			//	parser consumes from front, leaving unparsed remainder
			//	CRITICAL: without this, partial data between recv() calls is lost

		size_t	_bodyBytesRead;
			//	tracks how many body bytes have been appended so far
			//	used in BODY state to know when contentLength is reached

		std::string	_chunkSizeLine;
			//	accumulates the hex size line in chunked parsing
			//	"1a\r\n" → parsed to size_t → then read that many bytes

};

#endif


/*
	http request example:

	POST /upload HTTP/1.1
	Host: localhost
	Content-Length: 5

	hello

	-------------------------------------

	flow exmple:

	socket recv()
		↓
	HttpRequest::feed(chunk)
		↓
	parse progressively
		↓
	request complete?
		├── no → wait for more packets
		└── yes → "request ready"
*/
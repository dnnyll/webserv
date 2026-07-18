#ifndef		HTTPREQUEST_HPP
# define	HTTPREQUEST_HPP

#include	<string>
#include	<map>
#include	"../inc/HttpMessage.hpp"

enum	ParseState
{
	REQUEST_LINE,		//	parsing: "GET /index.html/1.1"
	HEADERS,			//	is parsing HTTP headers
	BODY,				//	is parsing request body
	CHUNKED,			//	parsing chunked transfer-encoding body
	COMPLETE,			//	is request fully parsed
	ERROR_STATE			//	malformed request
};


/*
enum HttpMethod
{
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
	METHOD_UNKNOWN
};

			HttpMessage
			───────────
			headers
			body
			version
		/         \
HttpRequest       HttpResponse
───────────       ────────────
method            statusCode
uri               statusMessage
_state            serialize()
_buffer           make()
contentLength
isChunked
feed()
isComplete()
hasError()

*/

class	HttpRequest :public HttpMessage
{
	public:
		std::string	method;
		//	http Method: GET POST DELETE

		std::string	uri;
		//	request URI/path
		//	"/images/cat.png"

		size_t		contentLength;
		//	content length extracted from Headers
		//	value of contentLength header converted to size_t
		//	0 if header not present or not applicable

		bool		isChunked;
		//	true when Transfer-Encoding: chunked is set
		//	when true: contentLength is ignored, use chunked parser

		size_t	_chunkSize;
		//	size of the current chunk being read
		//	0 means we are expecting a new chunk size line
		//	non-zero means we are reading chunk data

	private:
		ParseState		_state;
		//	current position in the state machine
		//	only feed() is allowed to advance this
		//	public code reads state via isComplete() / hasError() only
		//	for switch case acess to ParseState enum

		std::string		_buffer;
		//	raw bytes waiting to be parsed
		//	feed() appends incoming chunks here
		//	parser consumes from front, leaving unparsed remainder
		//	CRITICAL: without this, partial data between recv() calls is lost

		size_t			_bodyBytesRead;
		//	tracks how many body bytes have been appended so far
		//	used in BODY state to know when contentLength is reached

		// std::string		_chunkSizeLine;
		//	accumulates the hex size line in chunked parsing
		//	"1a\r\n" → parsed to size_t → then read that many bytes

		size_t			_maxBodySize;
		//	gets the value from -> Config -> AcceptHandler -> ClientHandler

	//	methods

	public:
		//	parsing/decoding
		HttpRequest();

		// parseFristLine(std::stringsteam input);
		// parseHeader(std::stringsteam input);
		// parseBody(std::stringsteam input);

		// static void		decode(HttpMessage &msg, int stop_at = HttpMessage::decoding_done);

		void	setMaxBodySize(size_t size);
		//	set MaxBodySize from Config parsing ServerBlock: client_max_body_size

		bool	getData(const std::string& chunk);
		// append new raw bytes from recv() to internal buffer
		// advances the parse state machine
		// returns true if request reaches COMPLETE state
		// safe to call multiple times with partial data
		// data arrives in chunks — never assume full request in one call
		
		bool	decode();

		//	Reset
		void	reset();

		bool	isComplete() const;
		//	returns true only when state == COMPLETE
		//	ClientHandler checks this after every feed() call

		bool	hasError() const;
		//	returns true when state == ERROR_STATE
		//	ClientHandler checks this to decide 400 or 505 response

	private:
		//	DecodeRequestLine
		bool	extractRequestLine(std::string &line);
		bool	splitRequestLine(const std::string &line);
		bool	validateRequestLine();
		void	decodeRequestLine();

		//	DecodeHeaders
		bool	extractHeaderLine(std::string &line, size_t &pos);
		bool	splitHeaderLine(const std::string &line);
		void	resolveBodyState();
		void	decodeHeaders();

		//	DecodeBody
		void	decodeBody();

		//	DecodeChunked
		bool	parseChunkSize();
		void	decodeChunked();
};

#endif


/*

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


new request
     │
     ▼
REQUEST_LINE
     │
     ▼
HEADERS
     │
     ├──────────────► ERROR_STATE
     │
     ├─ Content-Length
     │
     ▼
BODY
     │
     ▼
COMPLETE

or

REQUEST_LINE
     │
     ▼
HEADERS
     │
     ├─ Transfer-Encoding: chunked
     ▼
CHUNKED
     │
     ▼
COMPLETE
*/

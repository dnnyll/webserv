#ifndef		HTTPMESSAGE
#define		HTTPMESSAGE

#include	<string>
#include	<map>
#include	<iostream>

/*
	POST /upload HTTP/1.1
	Host: localhost:8080
	Content-Type: text/plain
	Content-Length: 5

	hello
*/

class	HttpMessage
{
	public:
		//	version			────────────────────────────────────────
		std::string	version;
			//	HTTP protocol version
			//	to verify that it's "HTTP/1.1" — anything else → ERROR_STATE 505
			//	request:  parsed from request line "HTTP/1.1"
			//	response: written into status line "HTTP/1.1"

		//	header			────────────────────────────────────────
		std::map<std::string, std::string> headers;
			//  HTTP headers — shared by request and response
			//  request:  parsed from incoming bytes
			//             headers["Host"]              = "localhost:8080"
			//             headers["Content-Length"]    = "42"
			//             headers["Connection"]        = "keep-alive"
			//             headers["Transfer-Encoding"] = "chunked"
			//  response: built by Router and serialize()
			//             headers["Content-Type"]      = "text/html"
			//             headers["Content-Length"]    = "42"
			//             headers["Connection"]        = "keep-alive"

		//	body			────────────────────────────────────────
		std::string	body;
			//  message body — shared by request and response
			//  request:  populated by feed() after headers complete
			//             POST upload data, CGI stdin
			//  response: populated by Router
			//             file contents, error page, CGI output

		//	constructors		────────────────────────────────────────
	public:
		virtual	~HttpMessage();
			//  virtual destructor — required when inheriting
			//  if you delete HttpRequest through HttpMessage*
			//  without this the wrong destructor runs → leak

	protected:
		HttpMessage();
			//  protected — HttpMessage is never instantiated directly
			//  only HttpRequest and HttpResponse call this
			//  initialises version to empty string
			//  headers and body are empty by default
};

#endif

// flow:

// accept()
//     │
//     ▼
// ClientHandler

//     │
//     ▼
// recv()

//     │
//     ▼
// HttpRequest::feed()

//     │
//     ├── _buffer += bytes
//     │
//     ├── parseRequestLine()
//     │       └── method/uri/version
//     │
//     ├── parseHeaders()
//     │       └── headers map
//     │
//     ├── parseBody()
//     │       └── body string
//     │
//     └── COMPLETE

//     │
//     ▼
// Router::handle(request)

//     │
//     ▼
// HttpResponse filled

//     │
//     ▼
// HttpResponse::encode()

//     │
//     ├── status line
//     ├── headers map
//     ├── CRLF
//     └── body

//     │
//     ▼
// send()

//     │
//     ▼
// TCP Socket



// fd → string _buffer → parse → HttpResponse → serialize() → string _writeBuf → fd

// ┌──────────┐    ┌────────────────┐    ┌─────────────┐    ┌────────────────┐
// │  recv()  │───▶│  _buffer       │───▶│  HttpRequest│───▶│  Router        │
// │  fd      │    │  std::string   │    │  feed()     │    │  route()       │
// └──────────┘    │  appended each │    │  state mach │    └───────┬────────┘
// 				   │  recv() call   │    └─────────────┘            │
// 				   └────────────────┘                               ▼
// ┌──────────┐    ┌────────────────┐    ┌─────────────────────────────────────┐
// │  send()  │◀───│  _writeBuf     │◀───│  HttpResponse::serialize()          │
// │  fd      │    │  std::string   │    │  uses stringstream INTERNALLY       │
// └──────────┘    │  offset track  │    │  returns std::string — done         │
// 				   └────────────────┘    └─────────────────────────────────────┘

// stringstream appears only inside serialize() as a local variable
// it builds the string and is then thrown away
// the persistent buffers are always plain std::string


// 			HttpMessage
// 			───────────
// 			headers
// 			body
// 			version
// 		/         \
// HttpRequest       HttpResponse
// ───────────       ────────────
// method            statusCode
// uri               statusMessage
// _state            decode()
// _buffer           make()
// contentLength
// isChunked
// feed()
// isComplete()
// hasError()
#ifndef		HTTPMESSAGE
#define		HTTPMESSAGE

#include	<string>
#include	<map>
#include	<iostream>
class	HttpMessage
{
	public:
		std::string	version;
		std::map<std::string, std::string> headers;
		std::string	body;
	public:
		virtual	~HttpMessage();
	protected:
		HttpMessage();
};

#endif

/*
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
*/

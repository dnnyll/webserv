#ifndef HTTPMESSAGE
#define HTTPMESSAGE

#include	<string>


/*
	POST /upload HTTP/1.1
	Host: localhost:8080
	Content-Type: text/plain
	Content-Length: 5

	hello
*/

Class	HttpMessage
{
	public:
		
		//	line fields ──────────────────────────────────
		std::string	firstLine[3];	//	contains: method; uri; version

		//	header fields ────────────────────────────────────────
		std::map<std::string, std::string> headers;
		//	all parsed headers stored here
		//	headers["Host"]           = "localhost:8080"
		//	headers["Content-Length"] = "42"
		//	headers["Connection"]     = "keep-alive"
		//	headers["Transfer-Encoding"] = "chunked"
		//	HTTP/1.1: Host header is REQUIRED — missing → 400

		//	body field ───────────────────────────────────────────
		std::stringstream	body;
		//	populated after BODY or CHUNKED state completes
		//	used for POST uploads, CGI stdin
		//	used for POST uploads/images
}

#endif
/*

GET    /index.html   HTTP/1.1
[0]       [1]           [2]

HTTP/1.1    200   OK
[0]         [1]   [2]

Class	HttpMessage
{
	public:
		//  ── shared fields ────────────────────────────────────────
		std::string	version;
			//  HTTP protocol version
			//  request:  parsed from request line "HTTP/1.1"
			//  response: written into status line "HTTP/1.1"

		std::map	headers;
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

		std::string	body;
			//  message body — shared by request and response
			//  request:  populated by feed() after headers complete
			//             POST upload data, CGI stdin
			//  response: populated by Router
			//             file contents, error page, CGI output
	public:
		virtual ~HttpMessage();
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
*/

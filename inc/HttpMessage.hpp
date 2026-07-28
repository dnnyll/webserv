#ifndef		HTTPMESSAGE_HPP
#define		HTTPMESSAGE_HPP

#include	<string>
#include	<map>
#include	<iostream>

// std::string inputTest = "	POST /upload HTTP/1.1
// 							Host: localhost:8080
// 							Content-Type: text/plain
// 							Content-Length: 5

// 							hello"


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

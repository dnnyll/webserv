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
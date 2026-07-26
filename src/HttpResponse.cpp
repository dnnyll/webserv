#include	"HttpResponse.hpp"
#include	<sstream>

//	constructor ──────────────────────────────────────────────────────

HttpResponse::HttpResponse(): statusCode("200"), statusMessage("OK"){}
//	temporary hardcoded default sucess code: 200; text: "OK"
//	default to 200 OK
//	body and headers are empty — Router fills them before serialize()


//	serialize ────────────────────────────────────────────────────────

std::string	HttpResponse::serialize() const
{
	//	temporary hardcoded response, so Alfavre can test loop
	//	real implementation will be added here by Daniefe2
	//
	//	real implementation must:
	//		build status line:    "HTTP/1.1 200 OK\r\n"
	//		iterate headers map:  "Content-Type: text/html\r\n"
	//		always add:           "Content-Length: {body.size()}\r\n"
	//		always add:           "Connection: keep-alive\r\n"
	//						   or "Connection: close\r\n"
	//		add blank line:       "\r\n"
	//		append body
	//
	return ("HTTP/1.1 200 OK\r\n\r\nh1");
	//	stub: returns hardcoded response
	//	Alfavre uses this to confirm the loop works end to end
	//	Daniefe2 replaces this with real implementation
}

HttpResponse	HttpResponse::make(int code, const std::string& message)
{
	HttpResponse	response;
	std::stringstream	ss;

	ss << code;
	response.statusCode = ss.str();
	response.statusMessage = message;
	response.body = message;
	return (response);
}


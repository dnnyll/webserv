/*
	This is responsible for building the server’s reply to the client. 
	It takes a status code, headers, and body, and converts everything into a properly 
	formatted HTTP response string.
	In the end, it produces the exact bytes that will be sent back over the socket.
*/

#include	"HttpResponse.hpp"

//	constructor ──────────────────────────────────────────────────────

HttpResponse::HttpResponse(): statusCode(200), statusMessage("OK"){}
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


//	make ─────────────────────────────────────────────────────────────

HttpResponse	HttpResponse::make(int code, const std::string& message)
{
	//	factory method for quick error/status responses
	//	Router calls this instead of building responses manually:
	//		return HttpResponse::make(404, "Not Found");
	//
	//	real implementation will build a minimal HTML body:
	//		"404 Not Found"
	//	and set Content-Type: text/html
	//
	HttpResponse	response;
	response.statusCode = code;
	response.statusMessage = message;
	response.body = message;
	//	stub: body is just the message string
	//	real implementation builds a proper HTML error page
	return (response);
}


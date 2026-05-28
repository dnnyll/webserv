/*
	This is responsible for building the server’s reply to the client. 
	It takes a status code, headers, and body, and converts everything into a properly 
	formatted HTTP response string.
	In the end, it produces the exact bytes that will be sent back over the socket.
*/

#include	"HttpResponse.hpp"

HttpResponse::HttpResponse(): statusCode(200), statusMessage("OK"){}
//	temporary hardcoded default sucess code: 200; text: "OK"

std::string	HttpResponse::serialize() const
{
	//	temporary hardcoded response, so Alfavre can test loop
	return ("HTTP/1.1 200 OK\r\n\r\nh1");
}
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
	std::string	statusLine;
	std::string headerStr;
	std::string	bodyStr;
	std::string	responseStr;


	//iter sur map de header pour remplir headerStr;
	statusLine = this->version + this->statusCode + " "
		+ this->statusMessage + "\r\n";
	headerStr = "Content-Type: " + this->headers["Content-Type"] + "\r\n";
	headerStr += "Content-Length: " + this->headers["Content-Length"] + "\r\n";
	headerStr += "Connection: " + this->headers["Connection"] + "\r\n";
	bodyStr = this

		this->body;

	//		iterate headers map:  "Content-Type: text/html\r\n"
	//		always add:           "Content-Length: {body.size()}\r\n"
	//		always add:           "Connection: keep-alive\r\n"
	//						   or "Connection: close\r\n"
	//		add blank line:       "\r\n"
	//		append body
	//
	return (/*??*/);
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


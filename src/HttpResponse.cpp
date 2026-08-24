#include	"HttpResponse.hpp"
#include	<sstream>

HttpResponse::HttpResponse(): statusCode("200"), statusMessage("OK"){}

std::string	HttpResponse::serialize() const
{
	std::string	statusLine;
	std::string	headerStr;


	statusLine = this->version + " " + this->statusCode + " "
		+ this->statusMessage + "\r\n";

	std::map<std::string, std::string>	serialize_headers = this->headers;
	std::stringstream lengthStream;
	lengthStream << body.size();
	serialize_headers["Content-Length"] = lengthStream.str();
	std::map<std::string, std::string>::const_iterator	iter = serialize_headers.begin();
	while (iter != serialize_headers.end())
	{
		headerStr += iter->first;
		headerStr += ": ";
		headerStr += iter->second;
		headerStr += "\r\n";
		iter++;
	}
	headerStr += "\r\n";

	return (statusLine + headerStr + this->body);
}

HttpResponse	HttpResponse::make(int code, const std::string& message)
{
	HttpResponse	response;
	std::stringstream	ss;

	ss << code;
	response.statusCode = ss.str();
	response.statusMessage = message;
	response.body = message;
	response.headers["Content-Type"] = "text/plain";
	return (response);
}


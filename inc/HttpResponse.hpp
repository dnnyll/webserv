#ifndef		HTTPRESPONSE_HTTP
# define	HTTPRESPONSE_HTTP

#include	<string>
#include	<map>
#include	"HttpMessage.hpp"

class	HttpResponse : public HttpMessage
{
	public:
		std::string	statusCode;
		std::string statusMessage;
	public:
		HttpResponse();
		std::string	serialize() const;
		static	HttpResponse make(int code, const std::string& message);
	};

#endif

	/*
		httpResponse builds the server response sent to client

		responsible for:
		- status code
		- status message
		- headers
		- response body
		- converting everything into raw HTTP text (using serialize())
	
		example:
		
		Example output:
		HTTP/1.0 200 OK
		Content-Length: 5

		hello     
*/

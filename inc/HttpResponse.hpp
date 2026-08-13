#ifndef		HTTPRESPONSE_HTTP
# define	HTTPRESPONSE_HTTP

#include	<string>
#include	<map>
#include	"HttpMessage.hpp"

enum	ResponseType
{
	CGI_PENDING,
	RESPONSE_READY
};

class	HttpResponse : public HttpMessage
{
	public:
		std::string				statusCode;
		std::string				 statusMessage;
	public:
		HttpResponse();
		std::string				serialize() const;
		static	HttpResponse	make(int code, const std::string& message);
	};

#endif

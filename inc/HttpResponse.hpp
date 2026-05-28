#ifndef		HTTPRESPONSE_HPP
# define	HTTPRESPONSE_HTTP

# include	<string>
# include	<map>

// class HttpResponse
// {
// 	public:
// 		HttpResponse();

// 	private:
// 		int									status;
// 		std::map<std::string, std::string>	headers;
// 		std::string							body;
// };

// #endif


class	HttpResponse
{
	public:
		int	statusCode;
		//	HTTP status code
		//	examples:	200, 404, etc

		std::string statusMessage;
		//	HTTP status text
		//	examples:	"ok" or "not found"

		std::map<std::string, std::string>	headers;
		//	response headers
		//	example:	headers["Content-Type"] = "text/html"

		std::string	body;
		//	response body sent to client

		httpResponse();
		//	initializes status code + status message
		//	example:	200 OK, 404 Not Found

		std::string	serialize() const;
		//	converts response object into raw bytes:
		//	// HTTP/1.0 200 OK\r\n
		//	Header: value\r\n
		//	\r\n
		//	body
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
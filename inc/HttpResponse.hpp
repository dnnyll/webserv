#ifndef		HTTPRESPONSE_HTTP
# define	HTTPRESPONSE_HTTP

# include	<string>
# include	<map>

class	HttpResponse
{
	public:
		//	response line fields ─────────────────────────────────

		int	statusCode;
		//	HTTP status code
		//	200, 201, 301, 302, 400, 403, 404, 405, 413, 500, 505

		std::string statusMessage;
		//	HTTP status text matching the code
		//	examples:	
		//	200 → "OK"
		//	404 → "Not Found"
		//	500 → "Internal Server Error"


		//	header fields ────────────────────────────────────────
		std::map<std::string, std::string>	headers;
		//	response headers
		//	example:
		//	response headers sent to client
		//	headers["Content-Type"]   = "text/html"
		//	headers["Content-Length"] = "42"
		//	headers["Connection"]     = "keep-alive" or "close"
		//	headers["Location"]       = "/new-path"  (redirects only)

		
		//	body field ───────────────────────────────────────────

		std::string	body;
		//	response body sent to client
		//	HTML page, file contents, error message, CGI output

	public:
		HttpResponse();
		//	default constructor
		//	initializes status code + status message
		//	example:	200 OK, 404 Not Found
		//	headers and body are empty — caller fills them

		std::string	serialize() const;
		//	converts response object into raw HTTP/1.1 bytes:
		//format:
		//		// HTTP/1.0 200 OK\r\n
		//		Header: value\r\n
		//		\r\n
		//		body
		//	always adds Content-Length from body.size()
		//	always adds Connection: keep-alive or close
		//	result written into ClientHandler::_writeBuf

		static HttpResponse make(int code, const std::string& message);
		//	factory method — builds a minimal response quickly
		//	Router uses this for error responses:
		//		return HttpResponse::make(404, "Not Found");
		//		return HttpResponse::make(405, "Method Not Allowed");
		//		return HttpResponse::make(500, "Internal Server Error");
		//	sets statusCode and statusMessage
		//	sets a plain text body matching the message
		//	caller can add headers after if needed
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
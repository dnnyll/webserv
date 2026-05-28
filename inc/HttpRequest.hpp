#ifndef		HTTPREQUEST_HPP
# define	HTTPREQUEST_HPP

# include	<string>
# include	<map>

// class HttpRequest
// {
// 	public:
// 		HttpRequest();

// 	private:
// 		std::string	method;
// 		std::string	uri;
// 		std::map<std::string, std::string> headers;
// 		std::string	body;
// };

// #endif

enum	ParseState
{
	REQUEST_LINE,		//	parsing: "GET /index.html/1.1"
	HEADERS,			//	is parsing HTTP headers
	BODY,				//	is parsing request body
	COMPLETE,			//	is request fully parsed
	ERROR_STATE			//	malformed request
};

class	HttpRequest
{
	public:
		std::string	method;
		//	GET POST DELETE

		std::string	uri;
		//	request URI/path
		//	"/images/cat.png"

		std::map<std::string, std::string> headers;
		//	all parsed headers
		//	headers["Host"] = "localhost:8080"

		std::string body;
		//	used for POST uploads/images

		ParseState state;
		//	curent parser state machine status

		size_t contentLength;
		//	content length extracted from Headers

	public:
		HttpRequest();

		bool feed(const std::string& chunk);
		//	used to append new raw bytes to parser
		//	advances parsing state machine
		//	returns true if request becomes COMPLETE
		
		bool isComplete() const;
		//	returns true when parsing is finished
};

#endif


/*
	http request example:

	POST /upload HTTP/1.1
	Host: localhost
	Content-Length: 5

	hello

	-------------------------------------

	flow exmple:

	socket recv()
		↓
	HttpRequest::feed(chunk)
		↓
	parse progressively
		↓
	request complete?
		├── no → wait for more packets
		└── yes → "request ready"
*/
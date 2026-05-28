#ifndef		ROUTER_HPP
#define		ROUTER_HPP

#include	"HttpRequest.hpp"
#include	"HttpResponse.hpp"
#include	"ServerConfig.hpp"

class	Router
{
public:

	Router();
		// stateless router constructor

	HttpResponse route(const HttpRequest& request, const ServerConfig& config);
		// parsed incoming request
		// server configuration selected
		// by host/ip/port

		// main routing function
		//
		// decides:
		// - which location matches
		// - allowed methods
		// - redirects
		// - static files
		// - CGI
		// - error responses
		//
		
		// takes parsed request + server configuration
		// returns fully built HTTP response
};

/*
	Router Responsibility

	input:
	HttpRequest (parsed request from client)
	ServerConfig (server rules + locations)

	core job:
	Decide what the request means and where it goes

	it handles:
		- route matching (/path → correct LocationConfig)
		- HTTP method validation (GET/POST/DELETE)
		- redirect logic
		- error decisions (404, 405, 500)
		- deciding static file vs CGI

	Output:
		HttpResponse (fully constructed response object)

	IMPORTANT:
		- does NOT read sockets
		- does NOT parse raw HTTP
		- does NOT execute system calls
		- ONLY decides "what response should be built"

*/

#endif
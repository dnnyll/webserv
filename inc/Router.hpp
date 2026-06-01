#ifndef		ROUTER_HPP
# define	ROUTER_HPP

#include	"HttpRequest.hpp"
#include	"HttpResponse.hpp"
#include	"Config.hpp"
#include	<vector>
#include	<iostream>

//	RouteResult carries the answer — holds the output
struct	RouteResult
{
	HttpResponse	response;
	//	fully built response — used when isCgi is false
	//	contains status, headers, body ready to serialize()

	bool	isCgi;
	//	false → response is ready, ClientHandler serializes + sends
	//	true  → ClientHandler must fork, exec, register CgiHandler

	std::string	cgiInterpreter;
	//	absolute path to CGI interpreter
	//	example: "/usr/bin/python3"
	//	only valid when isCgi is true

	std::string	cgiScriptPath;
	//	resolved filesystem path to the script
	//	example: "/var/www/cgi-bin/upload.py"
	//	only valid when isCgi is true

	RouteResult();
	//	default: isCgi = false, empty response
};

//	Router does the work — makes decisions
class	Router
{
	/*
		Router Responsibility
		─────────────────────
		input:
			HttpRequest             parsed request from client
			vector    all server blocks from config
			string ip + int port    which socket the connection arrived on

		core job:
			decide what the request means and where it goes

		steps:
			1. filter configs by ip:port → candidate ServerConfigs
			2. match Host header         → pick one ServerConfig
			3. match URI                 → pick LocationConfig
			exact match (isExact=true) checked first
			then longest prefix match
			4. validate HTTP method      → 405 if not in allowedMethods
			5. decide response source:
			redirect set             → 301/302 response
			CGI extension match      → RouteResult with isCgi=true
			GET static file          → read file, build 200
			GET directory            → index file or autoindex
			POST upload              → write file, build 201
			DELETE                   → delete file, build 200
			error                    → 400/403/404/405/413/500

		output:
			RouteResult
				isCgi = false → response fully built
				isCgi = true  → ClientHandler handles fork/pipe/exec

		rules:
			does NOT read sockets
			does NOT parse raw HTTP
			does NOT execute system calls (fork, exec, open)
			does NOT get instantiated — static method only
			ONLY decides what response should be built
			calls FileSystem free functions for disk operations
	*/
public:
	static	RouteResult	route(
		const HttpRequest& request,
		const std::vector<Config>& configs,
		const std::string& ip,
		int port);
		//	main routing function — never instantiate Router
		//	call as: Router::route(request, configs, ip, port)
		//	request  → fully parsed HttpRequest (isComplete() == true)
		//	configs  → all ServerConfigs from config file
		//	ip       → ip the connection arrived on (from AcceptHandler)
		//	port     → port the connection arrived on (from AcceptHandler)
		//	returns RouteResult — check isCgi before using response

private:
	Router();
	//	private constructor — prevents instantiation
	//	Router is never created as an object
};

#endif


// //	previous version attempt
// class	Router
// {
// public:

// 	Router();
// 		// stateless router constructor

// 	HttpResponse route(const HttpRequest& request, const Config& config);
// 		// parsed incoming request
// 		// server configuration selected
// 		// by host/ip/port

// 		// main routing function
// 		//
// 		// decides:
// 		// - which location matches
// 		// - allowed methods
// 		// - redirects
// 		// - static files
// 		// - CGI
// 		// - error responses
// 		//
		
// 		// takes parsed request + server configuration
// 		// returns fully built HTTP response
// };

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

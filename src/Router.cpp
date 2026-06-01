/*
	This is the decision-making core. 
	It takes a completed request and the server configuration, 
	then figures out what should happen (file serve, error, redirect, or CGI).
	It builds the correct HttpResponse based on those rules and returns it to the server loop.
*/

#include	"Router.hpp"

//	RouteResult constructor ───────────────────────────────────────────

RouteResult::RouteResult()
	: isCgi(false)
{
}
	//	default: not a CGI request
	//	response is a default 200 OK from HttpResponse constructor
	//	ClientHandler checks isCgi first before using response


//	Router::route ────────────────────────────────────────────────────

RouteResult Router::route(
	const HttpRequest& request,
	const std::vector<Config>& configs,
	const std::string& ip,
	int port)
{
	//  temporary hardcoded routing — Dev A can test the loop
	//  real implementation will be added here by Dev B
	//
	//  real implementation steps:
	//
	//  step 1 — filter configs by ip:port
	//      walk configs, keep only those whose listens contain ip:port
	//
	//  step 2 — pick ServerConfig by Host header
	//      request.headers["Host"] must match serverNames
	//      if no match → use first candidate (default server)
	//
	//  step 3 — pick LocationConfig by URI
	//      check isExact matches first
	//      then find longest prefix match against request.uri
	//      if no match → return make(404)
	//
	//  step 4 — validate method
	//      check request.method against location.allowedMethods
	//      if not listed → return make(405)
	//
	//  step 5 — decide response source
	//      if location.redirectCode != 0
	//          res.response = HttpResponse::make(301/302)
	//          res.response.headers["Location"] = location.redirectUri
	//          return res
	//      if URI extension in location.cgiExtensions
	//          res.isCgi = true
	//          res.cgiInterpreter = location.cgiExtensions[ext]
	//          res.cgiScriptPath  = resolved filesystem path
	//          return res
	//      if GET → FileSystem::readFile → build 200
	//      if POST → FileSystem::writeFile → build 201
	//      if DELETE → FileSystem::deleteFile → build 200
	//      error → HttpResponse::make(code, message)

	RouteResult	result;
		//	stub: returns default 200 OK
		//	Dev A uses this to confirm full request-response cycle works
	return (result);
}
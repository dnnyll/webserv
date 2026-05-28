/*
	This is the decision-making core. 
	It takes a completed request and the server configuration, 
	then figures out what should happen (file serve, error, redirect, or CGI).
	It builds the correct HttpResponse based on those rules and returns it to the server loop.
*/

#include	"Router.hpp"

Router::Router(){}

HttpResponse	Router::route(const HttpRequest&, const Config&)
{
	// temporary hardcoded routing

	HttpResponse	response;
	return (response);
}
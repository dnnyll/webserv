/*
	This part is responsible for taking raw incoming data from the client,
	gradually turning it into a structured HTTP request. 
	It keeps reading chunks until the full request (headers + body) is complete.
	In the end, it tells the system “I have everything, you can process this request now.”
*/

#include	"HttpRequest.hpp"

HttpRequest::HttpRequest(): state(REQUEST_LINE), contentLength(0){}
//	parser starts at request line, there's no body length known yet

bool	HttpRequest::feed(const std::string&)
{
	//	real parer will be added later
	return (false);
}

bool	HttpRequest::isComplete() const
{
	// only true when state == COMPLETED
	return (false);
}
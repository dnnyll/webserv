#include	"../inc/HttpMessage.hpp"

HttpMessage::HttpMessage() : version("HTTP/1.1")
{
}

HttpMessage::~HttpMessage()
{
	//	virtual — ensures correct destructor chain when deleting
	//	through a base class pointer
	//	string and map clean up their own memory automatically
}

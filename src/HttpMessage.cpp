#include	"HttpMessage.hpp"

//  ── constructor ──────────────────────────────────────────────────────
HttpMessage::HttpMessage()
{
	//	version, headers, body all default-constructed
	//	string → empty, map → empty
	//	subclasses set version during parsing or response building
}

//  ── destructor ───────────────────────────────────────────────────────
HttpMessage::~HttpMessage()
{
	//	virtual — ensures correct destructor chain when deleting
	//	through a base class pointer
	//	string and map clean up their own memory automatically
}
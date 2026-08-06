#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
// #include "CgiHandler.hpp"
#include "../inc/CgiLaunch.hpp"


RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

ResponseType RequestHandler::processRequest(HttpResponse &res, CgiInfo &cgi)
{
	this->_location = getLocation();
	//TODO (jules) verifier avec alexi le format des redir
	if (this->_location && !this->_location->redirect.empty())
	{
		res = HttpResponse::make(301, "Moved Permanently");
		res.headers["Location"] = this->_location->redirect;
		return (RESPONSE_READY);
	}
	resolveBuildConfig();
	if (checkMethod())
	{
		res = HttpResponse::make(405, "Method not allowed");
		return (RESPONSE_READY);
	}
	if (this->_request.body.size() > this->_effconf.client_max_body_size)
	{
		res = HttpResponse::make(413, "Payload Too Large");
		return (RESPONSE_READY);
	}
	if (this->_request.uri.empty() || this->_effconf.root.empty()
			|| this->_effconf.path.empty()) //TODO check utile avant le post ???
	{
		res = HttpResponse::make(500, "Internal Server Error");
		return (RESPONSE_READY);
	}
	this->_pathAbsolute = getPathAbsolute(this->_request.uri, this->_effconf.root,
			this->_effconf.path);
	std::cout << "[DEBUG ROUTER] pathAbsolute = [" << this->_pathAbsolute << "]" << std::endl;
	if (this->_pathAbsolute.empty())
	{
		res = HttpResponse::make(400, "Bad Request");
		return (RESPONSE_READY);
	}
	resolveFileSystem(); //TODO
	if (this->_effconf.status == CGI_NEEDED)
	{
		std::cout << "[PROCESSREQUEST] is a CGI" << std::endl;
		cgi = getCgiInfo();
		return (CGI_PENDING);
	}
	else if (this->_request.method == "POST")
	{	
		handlePost();
		res = this->_response;
	}
	else if (this->_request.method == "GET")
	{
		handleGet();
		res = this->_response;
	}
	else if (this->_request.method == "DELETE")
	{
		handleDelete();
		res = this->_response;
	}
	else
		res = HttpResponse::make(501, "Not Implemented");
	return (RESPONSE_READY);
}

//router
//	choisir la location la plus specifique 
//resolver
//	construire la config effective (root index autoindex method)
//filesystem
//	resoudre le chemin disque + stat/index/autoindex
//responder
//	construire la reponse

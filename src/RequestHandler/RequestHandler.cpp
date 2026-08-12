#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
// #include "CgiHandler.hpp"
#include "../inc/Cgi/CgiLaunch.hpp"


RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
}

ResponseType RequestHandler::processRequest(HttpResponse &res, CgiInfo &cgi)
{
	this->_location = getLocation();
	//TODO (jules) verifier avec alexi le format des redir
	//TODO (alexis) le format a changé : location a un redirect.code & une redirect.url -> gestion du code?
	if (this->_location && !this->_location->redirect_url.empty())
	{
		res = HttpResponse::make(301, "Moved Permanently");
		res.headers["Location"] = this->_location->redirect_url;
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
			|| this->_effconf.path.empty()) //TODO jules check utile avant le post ???
	{
		res = HttpResponse::make(500, "Internal Server Error");
		return (RESPONSE_READY);
	}
	this->_pathAbsolute = getPathAbsolute(this->_request.uri, this->_effconf.root);
	std::cout << "[DEBUG ROUTER] pathAbsolute = [" << this->_pathAbsolute << "]" << std::endl;
	if (this->_pathAbsolute.empty())
	{
		res = HttpResponse::make(400, "Bad Request");
		return (RESPONSE_READY);
	}
	resolveFileSystem();
	if (this->_effconf.status == CGI_NEEDED)
	{
		std::cout << "[PROCESSREQUEST] is a CGI" << std::endl;
		if (getCgiInfo(cgi))
		{
			res = this->_response;
			return (RESPONSE_READY);
		}
		return (CGI_PENDING);
	}
	else if (this->_effconf.status == REDIRECT)
	{
		this->_response = HttpResponse::make(301, "Moved Permanently");
		this->_response.headers["Location"] = this->_request.uri + "/";
		res = this->_response;
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

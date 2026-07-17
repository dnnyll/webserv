#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::processRequest()
{
	this->_location = getLocation();
	resolveBuildConfig();
	if (checkMethod())
	{
		this->_response = HttpResponse::make(405, "Method not allowed");
		return (_response);
	}
	if (this->_request.uri.empty() || this->_effconf.root.empty()
			|| this->_effconf.path.empty())
	{
		this->_response = HttpResponse::make(500, "Interal Server Error");
		return (_response);
	}
	_pathAbsolute = getPathAbsolute(this->_request.uri, this->_effconf.root,
			this->_effconf.path);
	resolveFileSystem();
	if (this->_request.method == "GET")
		handleGet();
	else if (this->_request.method == "POST")
		handlePost();
	else if (this->_request.method == "DELETE")
		handleDelete();
	else
		this->_response = HttpResponse::make(501, "Not Implemented");
	return (_response);
}

//router
//	choisir la location la plus specifique 
//resolver
//	construire la config effective (root index autoindex method)
//filesystem
//	resoudre le chemin disque + stat/index/autoindex
//responder
//	construire la reponse

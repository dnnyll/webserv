#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::processRequest()
{
	this->_location = getLocation();
	resolverBuildConfig();
	if (checkMethod())
		std::cout << " 405 method not allowed" << std::endl;
	if (this->_request.uri.empty() || this->_effconf.root.empty()
			|| this->_effconf.path.empty())
	{
		std::cout << "empty requestUri | locationRoot | locationPath" << std::endl;
		return (_response);
	}
	_pathAbsolute = getPathAbsolute(this->_request.uri, this->_effconf.root,
			this->_effconf.path);



	if (checkFileExist())
	{
		std::cout << "pas de file ou si directory pas dindex correspondant" << std::endl;
		//fonction de return
	}
	std::cout << "fin de process" << std::endl;
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

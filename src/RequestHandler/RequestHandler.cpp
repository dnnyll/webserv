#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::process()
{
	if (populateLocation()) //match la location la plus proche
	{
		//quel erreur ?
		std::cout << "erreur de populateLocation" << std::endl;
		//fonction de return
	}
	//fonction qui complete server+location avec root index autoindex
	if (checkMethod())
	{
		std::cout << "method not allowed" << std::endl;
		//405 methode not allowed
		//fonction de return
	}
	if (_request.uri.empty() || _location->root.empty() || _location->path.empty())
	{
		std::cout << "empty requestUri | locationRoot | locationPath" << std::endl;
		return (_response);
	}
	_pathAbsolute = getPathAbsolute(_request.uri, _location->root, _location->path);
	if (checkFileExist())
	{
		std::cout << "pas de file ou si directory pas dindex correspondant" << std::endl;
		//fonction de return
	}
	std::cout << "fin de process" << std::endl;
	return (_response);
}

router
	choisir la location la plus specifique 
resolver
	construire la config effective (root index autoindex method)
filesystem
	resoudre le chemin disque + stat/index/autoindex
responder
	construire la reponse

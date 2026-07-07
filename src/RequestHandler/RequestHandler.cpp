#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::process()
{
	if (setLocation())
	{
		//quel erreur ?
		std::cout << "erreur de setLocation" << std::endl;
		//fonction de return
	}
	if (_request.uri.empty() || _location->root.empty() || _location->path.empty())
	{
		std::cout << "empty requestUri | locationRoot | locationPath" << std::endl;
		return (_response);
	}
	_pathAbsolute = getPathAbsolute(_request.uri, _location->root, _location->path);
	if (checkPermission())
	{
		std::cout << "erreur de checkPermission" << std::endl;
		//fonction de return
	}
	if (checkFile())
	{
		std::cout << "pas de file ou si directory pas dindex correspondant" << std::endl;
		//fonction de return
	}
	//check si fichier ou dossier exist

	//s'assurer que les resso
	std::cout << "fin de process" << std::endl;
	return (_response);
}

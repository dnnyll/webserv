#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::process()
{
	if (!setLocation())
	{
		//quel erreur ?
	}
	//check des permissions dans cette location
	//check si fichier ou dossier exist

	//s'assurer que les resso
	std::cout << "fin de process" << std::endl;
	return (_response);
}

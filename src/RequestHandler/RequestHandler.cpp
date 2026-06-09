#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::process()
{
	matchLocation();
	//check des permissions dans cette location
	//check si fichier ou dossier exist

	//s'assurer que les resso
	return (_response);
}

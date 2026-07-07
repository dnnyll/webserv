#include "RequestHandler.hpp"

RequestHandler::RequestHandler(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
	std::cout << "Constructor with parameter RequestHandler create" << std::endl;
}

HttpResponse RequestHandler::process()
{
	if (populateLocation())
	{
		//quel erreur ?
		std::cout << "erreur de populateLocation" << std::endl;
		//fonction de return
	}
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

#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class	RequestHandler
{
	private:
		//const HttpRequest	&_request;
		//const ServerBlock	&_config;
		HttpResponse		_response;

		//pointeur sur la location qui correspond a la request
		//string du chemin officiel

		//methode priver a voir si toute necessaire
		//void	matchLocation();
		//void	checkPermision();
		//void	handleGet();
		//void	handlePost();
		//void	handleDelete();
		//bool executeCGI();
		//void	generateErrorPage(int statusCode);

	public:
		//constructeur pendre la reference de la request et de la config

		//une fonction publique que la boucle du serverManager appelera
		//HttpResponse process();
};

#endif

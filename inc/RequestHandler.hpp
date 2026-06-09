#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

class	RequestHandler
{
	private:
		const HttpRequest	&_request;
		const ServerBlock	&_config;
		HttpResponse		_response;

		Location			_location;
		//pointeur sur la location qui correspond a la request
		//string du chemin officiel

		//methode priver a voir si toute necessaire
		Location	matchLocation();
		//void	checkPermision();
		//void	handleGet();
		//void	handlePost();
		//void	handleDelete();
		//bool executeCGI();
		//void	generateErrorPage(int statusCode);

	public:
		RequestHandler(const HttpRequest &request, const ServerBlock &config);

		HttpResponse	process();
};

#endif

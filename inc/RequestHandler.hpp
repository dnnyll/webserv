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

		const Location		*_location;	//reference ou ptr ?????
		std::string			_pathAbsolute;

		int					setLocation();
		std::string			getPathAbsolute(std::string uri, std::string root,
								std::string path);
		int					checkPermission();
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

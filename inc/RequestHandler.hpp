#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

enum	EffectiveConfig
{
	FILE,
	INDEX_FOUND,
	AUTOINDEX,
	NOT_FOUND,
	FORBIDDEN,
	CGI_NEEDED,
	ERROR
};

typedef struct s_EffectiveConfig
{
	std::string					path;
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::vector<std::string>	methods;

} t_EffectiveConfig;

class	RequestHandler
{
	private:
		const HttpRequest	&_request;
		const ServerBlock	&_config;

		T_EffectiveConfig	_effconf;
		HttpResponse		_response;

		const Location		*_location;
		std::string			_pathAbsolute;

		//router
		int					getLocation();
		//void				verifier redirection();


		//resolver
		void	resolverBuildConfig();

		int					checkMethod();
		std::string			getPathAbsolute(std::string uri, std::string root,
								std::string path);

		//filesystem
		int					checkFileExist();

		//responder


		//void	handleGet();
		//void	handlePost();
		//void	handleDelete();
		//bool executeCGI();
		//void	generateErrorPage(int statusCode);

	public:
		RequestHandler(const HttpRequest &request, const ServerBlock &config);

		HttpResponse	processRequest();
};


#endif

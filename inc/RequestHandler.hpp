#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

enum	FileSystemStatus
{
	FILE_FOUND,
	DIRECTORY_LISTING,
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
	std::vector<std::string>	methods;
	bool						autoindex;
	FileSystemStatus			status;
	std::string					upload_store;
	size_t						client_max_body_size;
} t_EffectiveConfig;

class	RequestHandler
{
	private:
		const HttpRequest	&_request;
		const ServerBlock	&_config;

		t_EffectiveConfig	_effconf;
		HttpResponse		_response;

		const Location		*_location;
		std::string			_pathAbsolute;

		//router
		const Location*		getLocation();
		//void				verifier redirection();


		//resolver
		void				resolveBuildConfig();

		int					checkMethod();
		std::string			getPathAbsolute(std::string uri, std::string root,
								std::string path);

		//filesystem
		void				resolveFileSystemDirectory();
		void				resolveFileSystem();

		//responder
		void				handleGet();
		void				handlePost();
		void				handleDelete();

		//bool executeCGI();
		//void	generateErrorPage(int statusCode);

	public:
		RequestHandler(const HttpRequest &request, const ServerBlock &config);

		HttpResponse	processRequest();
};


#endif

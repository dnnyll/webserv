#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
// #include "CgiHandler.hpp"
#include "../inc/CgiLaunch.hpp"


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
	std::string					cgi_extension;
	std::string					cgi_path;
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

		//cgi
		CgiInfo				getCgiInfo();

		//responder
		void				handleGet();
		void				directoryListing();
		void				handlePost();
		void				handleDelete();

		//void	generateErrorPage(int statusCode);

	public:
		static std::string			getFileTypeFromPath(const std::string &path);
		RequestHandler(const HttpRequest &request, const ServerBlock &config);

		ResponseType	processRequest(HttpResponse &res, CgiInfo &cgi);
};


#endif

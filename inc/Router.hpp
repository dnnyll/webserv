#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"
#include "../inc//Cgi/CgiLaunch.hpp"


enum	FileSystemStatus
{
	FILE_FOUND,
	DIRECTORY_LISTING,
	NOT_FOUND,
	FORBIDDEN,
	CGI_NEEDED,
	REDIRECT,
	ERROR
};

typedef struct s_EffectiveConfig
{
	std::string							path;
	std::string							root;
	std::string							index;
	std::vector<std::string>			methods;
	bool								autoindex;
	FileSystemStatus					status;
	std::string							upload_store;
	size_t								client_max_body_size;
	std::map<std::string, std::string>	cgi_pass;
	std::string							path_extension;
} t_EffectiveConfig;

class	Router
{
	private:
		const HttpRequest	&_request;
		const ServerBlock	&_config;

		t_EffectiveConfig	_effconf;
		HttpResponse		_response;

		const Location		*_location;
		std::string			_pathAbsolute;
		const Location*		getLocation();
		void				resolveBuildConfig();
		int					checkMethod();
		std::string			getPathAbsolute(std::string uri, std::string root);

		//filesystem
		void				resolveFileSystemDirectory();
		void				resolveFileSystem();

		//cgi
		int					getCgiInfo(CgiInfo &ret);

		//responder
		HttpResponse		makeError(int code, const std::string &message);
		void				handleGet();
		void				directoryListing();
		void				handlePost();
		void				handleDelete();

	public:
		static std::string			getFileTypeFromPath(const std::string &path);
		static HttpResponse		makeError(int code, const std::string &message, const ServerBlock &config);
		Router(const HttpRequest &request, const ServerBlock &config);

		ResponseType				processRequest(HttpResponse &res, CgiInfo &cgi);
};

#endif

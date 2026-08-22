#include "Router.hpp"
#include "HttpResponse.hpp"
// #include "CgiHandler.hpp"
#include "../inc/Cgi/CgiLaunch.hpp"
#include <fstream>
#include <sstream>


Router::Router(const HttpRequest &request, const ServerBlock &config) :
	_request(request), _config(config)
{
}

HttpResponse	Router::makeError(int code, const std::string &message, const ServerBlock &config)
{
	HttpResponse	response = HttpResponse::make(code, message);
	std::map<int, std::string>::const_iterator	it = config.error_pages.find(code);
	std::string	root;
	std::string	path;

	if (it == config.error_pages.end())
		return (response);

	path = it->second;
	if (path.find("..") != std::string::npos)
		return (response);
	root = config.root;
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	path = root + path;

	std::ifstream	file(path.c_str());
	if (!file.is_open())
		return (response);

	std::stringstream	ss;
	ss << file.rdbuf();
	response.body = ss.str();
	response.headers["Content-Type"] = "text/html";
	return (response);
}

HttpResponse	Router::makeError(int code, const std::string &message)
{
	return (makeError(code, message, _config));
}

ResponseType Router::routingResponse(HttpResponse &res, CgiInfo &cgi)
{
	this->_location = getLocation();
	//format redir a changer dans le parsing config
	//mais pas pris en compte pour le router toute les redirections ont le meme code
	if (this->_location && !this->_location->redirect_url.empty())
	{
		res = HttpResponse::make(301, "Moved Permanently");
		res.headers["Location"] = this->_location->redirect_url;
		return (RESPONSE_READY);
	}
	resolveBuildConfig();
	if (checkMethod())
	{
		res = makeError(405, "Method not allowed");
		return (RESPONSE_READY);
	}
	if (this->_request.body.size() > this->_effconf.client_max_body_size)
	{
		res = makeError(413, "Payload Too Large");
		return (RESPONSE_READY);
	}
	if (this->_request.uri.empty() || this->_effconf.root.empty()
			|| this->_effconf.path.empty())
	{
		res = makeError(500, "Internal Server Error");
		return (RESPONSE_READY);
	}
	this->_pathAbsolute = getPathAbsolute(this->_request.uri, this->_effconf.root);
	std::cout << "[DEBUG ROUTER] pathAbsolute = [" << this->_pathAbsolute << "]" << std::endl;
	if (this->_pathAbsolute.empty())
	{
		res = makeError(400, "Bad Request");
		return (RESPONSE_READY);
	}
	resolveFileSystem();
	if (this->_effconf.status == CGI_NEEDED)
	{
		std::cout << "[PROCESSREQUEST] is a CGI" << std::endl;
		if (getCgiInfo(cgi))
		{
			res = this->_response;
			return (RESPONSE_READY);
		}
		return (CGI_PENDING);
	}
	else if (this->_request.method == "POST")
	{	
		handlePost();
		res = this->_response;
	}
	else if (this->_request.method == "GET")
	{
		handleGet();
		res = this->_response;
	}
	else if (this->_request.method == "DELETE")
	{
		handleDelete();
		res = this->_response;
	}
	else
		res = makeError(501, "Not Implemented");
	return (RESPONSE_READY);
}

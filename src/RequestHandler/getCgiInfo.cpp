#include "RequestHandler.hpp"
// #include "CgiHandler.hpp"
#include "../inc/CgiLaunch.hpp"
#include <sstream>

static std::string	sizetToString(size_t number)
{
	std::stringstream	sizetStream;
	
	sizetStream << number;
	return sizetStream.str();
}

CgiInfo	RequestHandler::getCgiInfo()
{
	CgiInfo	ret;

	ret.scriptPath = this->_pathAbsolute;
	ret.interpreterPath = this->_effconf.cgi_path;
	//TODO (jules) verifier dir avec stat ? ou execve?
	size_t directory_path = this->_pathAbsolute.rfind('/');
	if (directory_path == std::string::npos)
		ret.workingDirectory = "/";
	else
		ret.workingDirectory = this->_pathAbsolute.substr(0, directory_path);

	std::string	query_string;
	std::string	script_name;

	ret.env.push_back("REQUEST_METHOD=" + this->_request.method);
	size_t	start_query = this->_request.uri.find('?');
	if (start_query == std::string::npos)
	{
		query_string = "";
		script_name = this->_request.uri;
	}
	else
	{
		query_string = this->_request.uri.substr(start_query + 1);
		script_name = this->_request.uri.substr(0, start_query);
	}
	ret.env.push_back("QUERY_STRING=" + query_string);
	if (this->_request.contentLength)
		ret.env.push_back("CONTENT_LENGTH="
			+ sizetToString(this->_request.contentLength));
	std::map<std::string, std::string>::const_iterator	iter =
		this->_request.headers.find("Content-Type");
	if (iter != this->_request.headers.end())
		ret.env.push_back("CONTENT_TYPE=" + iter->second);
	ret.env.push_back("SCRIPT_NAME=" + script_name);
	ret.env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	ret.env.push_back("SERVER_NAME=" + this->_config.host);
	ret.env.push_back("SERVER_PORT=" + sizetToString(this->_config.port));
	ret.env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	ret.env.push_back("REDIRECT_STATUS=200");
	return ret;
}

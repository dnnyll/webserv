#include "Router.hpp"
// #include "CgiHandler.hpp"
#include "../inc/Cgi/CgiLaunch.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <cctype>

static std::string  sizetToString(size_t number)
{
	std::stringstream   sizetStream;
	sizetStream << number;
	return sizetStream.str();
}

int Router::getCgiInfo(CgiInfo &ret)
{
	//cgiCheck before all
	struct stat statbuf;
	if  (stat(this->_pathAbsolute.c_str(), &statbuf) == -1)
	{
		this->_response = HttpResponse::make(404, "Not Found");
		return (1);
	}
	else if (access(this->_pathAbsolute.c_str(), X_OK) == -1)
	{
		this->_response = HttpResponse::make(403, "Forbidden");
		return (1);
	}

	ret.scriptPath = this->_pathAbsolute;
	ret.interpreterPath = this->_effconf.cgi_pass[this->_effconf.path_extension];

	size_t directory_path = this->_pathAbsolute.rfind('/');
	if (directory_path == std::string::npos)
	{
		ret.workingDirectory = "/";
		ret.execScriptName = this->_pathAbsolute;
	}
	else
	{
		ret.workingDirectory = this->_pathAbsolute.substr(0, directory_path);
		ret.execScriptName = this->_pathAbsolute.substr(directory_path + 1);
	}

	std::string query_string;
	std::string env_script_name;

	ret.env.push_back("REQUEST_METHOD=" + this->_request.method);
	size_t  start_query = this->_request.uri.find('?');
	if (start_query == std::string::npos)
	{
		query_string = "";
		env_script_name = this->_request.uri;
	}
	else
	{
		query_string = this->_request.uri.substr(start_query + 1);
		env_script_name = this->_request.uri.substr(0, start_query);
	}
	ret.env.push_back("QUERY_STRING=" + query_string);
	if (this->_request.contentLength)
		ret.env.push_back("CONTENT_LENGTH="
			+ sizetToString(this->_request.contentLength));

	std::map<std::string, std::string>::const_iterator  iter =
		this->_request.headers.find("Content-Type");
	if (iter != this->_request.headers.end())
		ret.env.push_back("CONTENT_TYPE=" + iter->second);

	//	convert every remaining header into HTTP_<NAME>, per RFC 3875:
	//	dashes -> underscores, uppercased. Content-Type/Content-Length
	//	already have dedicated variables above, skip them here.
	for (std::map<std::string, std::string>::const_iterator hit = this->_request.headers.begin();
		hit != this->_request.headers.end(); ++hit)
	{
		const std::string &key = hit->first;

		if (key == "Content-Type" || key == "Content-Length")
			continue ;

		std::string name = "HTTP_";
		for (size_t i = 0; i < key.size(); i++)
		{
			if (key[i] == '-')
				name += '_';
			else
				name += std::toupper(static_cast<unsigned char>(key[i]));
		}
		ret.env.push_back(name + "=" + hit->second);
	}

	ret.env.push_back("SCRIPT_NAME=" + env_script_name);
	ret.env.push_back("PATH_INFO=" + env_script_name);
	ret.env.push_back("REQUEST_URI=" + this->_request.uri);
	ret.env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	ret.env.push_back("SERVER_NAME=" + this->_config.host);
	ret.env.push_back("SERVER_PORT=" + sizetToString(this->_config.port));
	ret.env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	ret.env.push_back("REDIRECT_STATUS=200");
	return 0;
}
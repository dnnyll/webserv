#include "RequestHandler.hpp"
#include <sstream>
#include <dirent.h>

static std::string	escapeHtml(const std::string &str)
{
	std::string ret;
	size_t	i = 0;

	while (i < str.size())
	{
		switch (str[i])
		{
			case '&':
				ret += "&amp;";
				break;
			case '<':
				ret += "&lt;";
				break;
			case '>':
				ret += "&gt;";
				break;
			case '"':
				ret += "&quot;";
				break;
			default:
				ret += str[i];
		}
		i++;
	}
	return ret;
}

void	RequestHandler::directoryListing()
{
	std::cout << "DIRECTORY LISTING " << std::endl;
	DIR	*dir = opendir(_pathAbsolute.c_str());
	if (!dir)
	{
		this->_response = HttpResponse::make(500, "Internal Server Error");
		return;
	}

	std::string	uriDL = _request.uri;
	if (uriDL.empty() || uriDL[uriDL.size() - 1] != '/')
		uriDL += "/";

	std::string htmlStr;
	htmlStr += "<html><head><title>Index of " + escapeHtml(uriDL)
		+ "</title></head><body>";
	htmlStr += "<h1>Index of " + escapeHtml(uriDL) + "</h1><ul>";

	struct dirent *entry;
	entry = readdir(dir);
	while (entry != NULL)
	{
		std::string name = entry->d_name;
		entry = readdir(dir);
		if (name == ".")
			continue;
		htmlStr += "<li><a href=\"" + escapeHtml(uriDL + name) + "\">"
			+ escapeHtml(name) + "</a></li>";
	}
	closedir(dir);

	htmlStr += "</ul></body></html>";

	std::stringstream lengthStream;
	lengthStream << htmlStr.size();

	this->_response = HttpResponse::make(200, "OK");
	this->_response.body = htmlStr;
	this->_response.headers["Content-Length"] = lengthStream.str();
	this->_response.headers["Content-Type"] = "text/html";
}

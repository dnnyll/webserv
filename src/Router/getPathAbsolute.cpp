#include "Router.hpp"

std::string	Router::getPathAbsolute(std::string uri, std::string root)
{
	//query string si cgi
	size_t	npos = uri.find('?');
	if (npos != std::string::npos)
		uri = uri.substr(0, npos);


    if (uri.find("..") != std::string::npos)
		return ("");
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	return (root + uri); // endrit + /files/hello.txt
}

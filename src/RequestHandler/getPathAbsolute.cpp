#include "RequestHandler.hpp"

std::string	RequestHandler::getPathAbsolute(std::string uri, std::string root)
{
    if (uri.find("..") != std::string::npos)
		return ("");
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	return (root + uri); // endrit + /files/hello.txt
}

#include "Router.hpp"
#include <string>

std::string	Router::getFileTypeFromPath(const std::string &path)
{
	size_t	dotPos = path.rfind('.');

	if (dotPos == std::string::npos) //val de npos == -1
		return ("");
	return (path.substr(dotPos + 1));
}


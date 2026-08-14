#include "Router.hpp"
#include <cstdio>
#include <unistd.h>

static std::string	getParentDirectory(const std::string &path)
{
	size_t	pos = path.rfind('/');
	if (pos == std::string::npos)
		return ".";
	if (pos == 0)
		return "/";
	return path.substr(0, pos);
}

void	Router::handleDelete()
{
	switch (this->_effconf.status)
	{
		case FILE_FOUND:
		{
			std::string	parentDir = getParentDirectory(this->_pathAbsolute);
			if (access(parentDir.c_str(), W_OK) == -1)
			{
				this->_response = makeError(403, "Forbidden");
				break;
			}
			if (std::remove(this->_pathAbsolute.c_str()))
				this->_response = makeError(500, "Internal Server Error");
			else
				this->_response = HttpResponse::make(204, "No Content");
			break;
		}
		case DIRECTORY_LISTING: //normalement impossible
			this->_response = makeError(403, "Forbidden");
			break;
		case NOT_FOUND:
			this->_response = makeError(404, "Not Found");
			break;
		case FORBIDDEN:
			this->_response = makeError(403, "Forbidden");
			break;
		case CGI_NEEDED:
			//impossible dans ce contexte
			break;
		case REDIRECT:
			//Impossible dans ce contexte
			break;
		case ERROR:
			this->_response = makeError(500, "Internal Server Error");
			break;
	}
}

#include "RequestHandler.hpp"
#include <sys/stat.h>

void	RequestHandler::resolveFileSystem()
{
	struct stat	statbuf;

	if (stat(_pathAbsolute.c_str(), &statbuf) == -1)
	{
		if (errno == ENOENT)
			this->_effconf.status = NOT_FOUND;
		else if (errno == EACCES)
			this->_effconf.status = FORBIDDEN;
		else
			this->_effconf.status = ERROR;
		return ;
	}
	if (S_ISREG(statbuf.st_mode))
		this->_effconf.status = FILE_FOUND;
	else if (S_ISDIR(statbuf.st_mode))	//directory
	{
		if (this->_request.method == "DELETE")
			this->_effconf.status = FORBIDDEN;
		else
			resolveDirectory();
	}
	else
		this->_effconf.status = ERROR;
}

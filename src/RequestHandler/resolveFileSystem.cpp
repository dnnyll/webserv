#include "RequestHandler.hpp"
#include <sys/stat.h>

void	RequestHandler::resolveFileSystem()
{
	struct stat	statbuf;

	if (stat(this->_pathAbsolute.c_str(), &statbuf) == -1)
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
	{
		std::string path_extension = getFileTypeFromPath(this->_pathAbsolute);
		if (this->_effconf.cgi_extension == path_extension 
				&& this->_effconf.cgi_extension != "")
			this->_effconf.status = CGI_NEEDED;
		else
			this->_effconf.status = FILE_FOUND;
	}
	else if (S_ISDIR(statbuf.st_mode))
		resolveFileSystemDirectory();
	else
		this->_effconf.status = ERROR;
}

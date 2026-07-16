#include "RequestHandler.hpp"

void	RequestHandler::resolveDirectory()
{
	struct stat	statbuf;

	if (!this->_effconf.index.empty())
	{
		if (this->_effconf.autoindex)
			this->_effconf.status = DIRECTORY_LISTING;
		else
			this->_effconf.status = FORBIDDEN;
		return ;
	}

	std::string	indexPath = this->_pathAbsolute;

	if (indexPath.back() != '/')
		indexPath += "/";
	indexPath += this->_effconf.index;
	if (stat(indexPath.c_str(), &statbuf) == -1)
	{
		if (this->_effconf.autoindex)
			this->_effconf.status = DIRECTORY_LISTING;
		else
			this->_effconf.status = FORBIDDEN;
		return ;
		
	}
	if (!S_ISREG(statbuf.st_mode))
	{
		this->_effconf.status = ERROR;
		return ;
	}
	this->_pathAbsolute = indexPath;
	this->_effconf.status = FILE_FOUND;
}

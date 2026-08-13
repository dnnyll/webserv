#include "Router.hpp"
#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <map>

//TODO (JULES) reunir dans un fichier toute les fonctions de print
void printCgiPass(const std::map<std::string, std::string>& cgi_pass)
{
	std::map<std::string, std::string>::const_iterator it;
	for (it = cgi_pass.begin(); it != cgi_pass.end(); ++it)
		std::cout << "ext=[" << it->first << "] interpreter=[" << it->second << "]" << std::endl;
}

void	Router::resolveFileSystem()
{
	struct stat	statbuf;

	if (stat(this->_pathAbsolute.c_str(), &statbuf) == -1)
	{
		std::cout << "PATH ABSOLUTE not working "
			<< this->_pathAbsolute << std::endl; //TODO jules a supp ou non debug?
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
		if (access(this->_pathAbsolute.c_str(), R_OK) == -1)
		{
			this->_effconf.status = FORBIDDEN;
			return;
		}
		this->_effconf.path_extension = "." + getFileTypeFromPath(this->_pathAbsolute);
		//TODO (jules) sans le point core dump
		std::cout << "PATH_EXTENSION : " << this->_effconf.path_extension << std::endl;
		std::cout << "PRINT MAP CGI : " << std::endl;
		printCgiPass(this->_effconf.cgi_pass);
		if (this->_effconf.cgi_pass.count(this->_effconf.path_extension) > 0)
		{
			std::cout << "CGI!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;//TODO
			if (access(this->_pathAbsolute.c_str(), X_OK) == -1)
			{
				this->_effconf.status = FORBIDDEN;
			}
			this->_effconf.status = CGI_NEEDED;
		}
		else
			this->_effconf.status = FILE_FOUND;
	}
	else if (S_ISDIR(statbuf.st_mode))
		resolveFileSystemDirectory();
	else
		this->_effconf.status = ERROR;
}

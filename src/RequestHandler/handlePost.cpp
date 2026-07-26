#include "RequestHandler.hpp"
#include <sys/stat.h>
#include <fstream>
#include <unistd.h>

static std::string	getFileNameFromUri(const std::string &uri)
{
	size_t	indexStart = uri.rfind('/');

	if (indexStart == std::string::npos)
		return ("");
	return (uri.substr(indexStart + 1));
}

static bool	isValidFilename(const std::string &filename)
{
	if (filename.empty() || filename == ".")
		return 0;
	if (filename.find("..") != std::string::npos)
		return 0;
	return 1;
}

void	RequestHandler::handlePost()
{
	//TODO si uri correspond a CGI extension letsgo executeCGI
	if (this->_effconf.upload_store.empty())
	{
		this->_response = HttpResponse::make(403, "Forbidden");
		return ;
	}
	std::string filename = getFileNameFromUri(this->_request.uri);
	if (!isValidFilename(filename))
	{
		this->_response = HttpResponse::make(400, "Bad Request");
		return ;
	}
	struct stat	dirStat;
	if (stat(this->_effconf.upload_store.c_str(), &dirStat) == -1
			|| !S_ISDIR(dirStat.st_mode))
	{
		this->_response = HttpResponse::make(404, "Not Found");
		return ;
	}
	this->_pathAbsolute = this->_effconf.upload_store;
	if (this->_pathAbsolute[this->_pathAbsolute.size() - 1] != '/')
		this->_pathAbsolute += '/';
	this->_pathAbsolute += filename;
	bool alreadyExist = 0;
	if (!access(this->_pathAbsolute.c_str(), F_OK))
		alreadyExist = 1;
	std::ofstream	outFile(this->_pathAbsolute.c_str());
	if (!outFile.is_open())
	{
		this->_response = HttpResponse::make(500, "Internal Server Error");
		return ;
	}
	outFile << this->_request.body;
	outFile.close();
	if(alreadyExist)
		this->_response = HttpResponse::make(200, "OK");
	else
		this->_response = HttpResponse::make(201, "Created");

}

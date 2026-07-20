#include "RequestHandler.hpp"
#include <fstream>
#include <sstream>

static std::string	getFileTypeFromPath(const std::string &path)
{
	size_t	dotPos = path.rfind('.');

	if (dotPos == std::string::npos) //val de npos == -1
		return ("");
	return (path.substr(dotPos + 1));
}

//mime multimedia internet mail extension
static std::string	getContentType(const std::string &path)
{
	std::map<std::string, std::string> mimeTypes;
	mimeTypes["html"] = "text/html";
	mimeTypes["css"] = "text/css";
	mimeTypes["js"] = "application/javascript";
	mimeTypes["png"] = "image/png";
	mimeTypes["jpg"] = "image/jpeg";
	mimeTypes["txt"] = "text/plain";

	std::string fileType = getFileTypeFromPath(path);
	std::map<std::string, std::string>::const_iterator iter = mimeTypes.find(fileType);
	if (iter == mimeTypes.end())
		return ("application/octet-stream");
	return (iter->second);
}


void	RequestHandler::handleGet()
{

	switch (this->_effconf.status)
	{
		case FILE_FOUND:
		{
			std::ifstream	file(_pathAbsolute.c_str());
		
			if (!file.is_open())
			{
				this->_response = HttpResponse::make(500, "Internal Server Error");
				break;
			}
			std::stringstream	buffer;
			buffer << file.rdbuf();
			std::string	content = buffer.str();
			
			std::stringstream lengthStream;
			lengthStream << content.size();

			this->_response = HttpResponse::make(200, "OK");
			this->_response.body = content;
			this->_response.headers["Content-Length"] = lengthStream.str();
			this->_response.headers["Content-Type"] 
				= getContentType(this->_pathAbsolute);


			//version	written into status line "HTTP/1.1"
			//body
				//populated by Router
					//file contents, error page, CGI output

			break;
		}
		case DIRECTORY_LISTING:
			this->_response = HttpResponse::make(501, "Not Implemented"); //TODO list
			break;
		case NOT_FOUND:
			this->_response = HttpResponse::make(404, "Not Found");
			break;
		case FORBIDDEN:
			this->_response = HttpResponse::make(403, "Forbidden");
			break;
		case CGI_NEEDED:
			//executeCGI(); //TODO
			break;
		case ERROR:
			this->_response = HttpResponse::make(500, "Internal Server Error");
			break;

	}
}

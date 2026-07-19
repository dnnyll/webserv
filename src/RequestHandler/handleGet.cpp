#include "RequestHandler.hpp"
#include <fstream>
#include <sstream>

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
			this->_response = HttpResponse::make(200, "OK");
			this->_response.body = content;
			break;
		}
//		case DIRECTORY_LISTING:
//			this->_response = HttpResponse::make(/*code*/, /*message*/);
//			break;
//		case NOT_FOUND:
//			this->_response = HttpResponse::make(/*code*/, /*message*/);
//			break;
//		case FORBIDDEN:
//			this->_response = HttpResponse::make(/*code*/, /*message*/);
//			break;
//		case CGI_NEEDED:
//			//executeCGI();
//			break;
//		case ERROR:
//			this->_response = HttpResponse::make(/*code*/, /*message*/);
//			break;

	}
}

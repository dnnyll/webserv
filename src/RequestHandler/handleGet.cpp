#include "RequestHandler.hpp"
#include <fstream>
#include <sstream>

void	RequestHandler::handleGet()
{
	std::ifstream	file(_pathAbsolute.c_str());

	if (!file.is_open())
	{
		//sorry not sorry
	}
	std::stringstream	buffer;
	buffer << file.rdbuf();
	std::string	content = buffer.str();

//	switch (this->_effconf.status)
//	{
//		case FILE_FOUND:
//			this->_response = HttpResponse::make(/*code*/, /*message*/);
//			break;
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
//
//	}
}

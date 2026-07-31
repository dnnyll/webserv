#include "RequestHandler.hpp"
#include "CgiHandler.hpp"

CgiInfo	RequestHandler::getCgiInfo()
{
	CgiInfo	ret;

	ret.scriptPath = this->_pathAbsolute;
	ret.interpreterPath = this->_effconf.cgi_path;
	//ret.env a gerer
	return ret;
}

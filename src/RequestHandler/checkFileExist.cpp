#include "RequestHandler.hpp"
#include <sys/stat.h>

int	RequestHandler::checkFileExist()
{
	struct stat	statbuf;

	if (stat(_pathAbsolute.c_str(), &statbuf) == -1)
	{
		if (errno == ENOENT)
			return (1); //not found 404
		if (errno == EACCES)
			return (1); //forbidden 403
		return 1;//???????????
	}
	if (S_ISREG(statbuf.st_mode))
		return (0); //fichier
	else if (S_ISDIR(statbuf.st_mode))	
	{
		//dossier 
		//index autoindex etc
		//si index a cette location je prend 
		//sinon index du server config
		//si aucun des 2 nexiste 
		//alors autoindex
		//si on
		//		listing
		//si off
		//		403
		return 0;
	}
	else
		return 1; //403
}

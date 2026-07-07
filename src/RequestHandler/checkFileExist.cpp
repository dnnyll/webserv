#include "RequestHandler.hpp"

int	RequestHandler::checkFileExist()
{
	struct stat	statbuf;

	if (stat(_pathAbsolute.c_str(), &statbuf) == -1)
	{
		if (errno == ENOENT)
			return (1); //not found
		if (errno == EACCES)
			return (1); //forbidden
		return INTERNAL_SERVER_ERROR;//???????????
	}
	if (S_ISREG(statbuf.st_mode))
		return (0); //fichier
	else if (S_ISDIR(statbuf.st_mode))	
	{
		//dossier 
		//index autoindex etc
	}


	
//	URI
// │
// ▼
//Construire le chemin physique
// │
// ▼
//stat(path)
// │
// ├── erreur ENOENT → 404
// ├── erreur EACCES → 403
// │
// ▼
//Est-ce un fichier ?
// │
// ├── Oui → OK, servir ce fichier
// │
// └── Non (c'est un dossier)
//         │
//         ▼
//La location possède un index ?
//         │
//         ├── Oui
//         │      essayer chaque index (index.html, index.htm, ...)
//         │      │
//         │      ├── trouvé → servir cet index
//         │      └── aucun trouvé → continuer
//         │
//         ▼
//Le serveur possède un index par défaut ?
//         │
//         ├── Oui → essayer de le trouver
//         │
//         ▼
//Autoindex activé ?
//         │
//         ├── Oui → générer le listing
//         └── Non → 403 (ou selon les exigences du projet)
}

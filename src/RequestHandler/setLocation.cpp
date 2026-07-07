#include "RequestHandler.hpp"

static size_t	getCharMatching(std::string requestLocation, std::string configLocation)
{
	size_t	i = 0;
	while (requestLocation[i] == configLocation[i])
		i++;
	return (i);
}

int	RequestHandler::setLocation()
{
	std::vector<Location>::const_iterator iter = _config.locations.begin();
	size_t	tempNbChar = 0;
	size_t	nbCharLocation = 0;
	const Location *actLocation = NULL;

	while (iter != _config.locations.end())
	{
		tempNbChar = getCharMatching(_request.uri, iter->path);//!!!!!!!!!!!!!!!!!!
		if (tempNbChar > nbCharLocation)
		{
			nbCharLocation = tempNbChar;
			actLocation = &(*iter);
			if (actLocation->path == _request.uri)
				break;
		}
		iter++;
	}
	if (!actLocation)
	{
		if(!_config.root.empty())
		{
			_location = &(*_config.locations.begin());//retour de la premiere location /
			return (0);
		}
		else
		{
			std::cout << "no root no location matching" << std::endl;
			return (1); //matcher l'erreur
		}
	}
	_location = actLocation;
	return (0);
}

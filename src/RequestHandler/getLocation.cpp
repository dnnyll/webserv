#include "RequestHandler.hpp"

static size_t	getDirectoryMatching(std::string requestLocation,
		std::string configLocation)
{
	size_t	i = 0;
	size_t	dir = 0;
	while (requestLocation[i] == configLocation[i])
	{
		i++;
		if (requestLocation[i] == '/')
			dir++;
	}
	return (dir);
}

int	RequestHandler::getLocation()
{
	std::vector<Location>::const_iterator iter = _config.locations.begin();
	size_t	tempNbDir = 0;
	size_t	nbDirLocation = 0;
	const Location *actLocation = NULL;

	while (iter != _config.locations.end())
	{
		tempNbDir = getDirectoryMatching(_request.uri, iter->path);
		if (tempNbDir > nbDirLocation)
		{
			nbDirLocation = tempNbDir;
			actLocation = &(*iter);
			if (actLocation->path == _request.uri)
				break;
		}
		iter++;
	}
	//dir = 1?
	return (actLocation);
}

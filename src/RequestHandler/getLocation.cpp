#include "RequestHandler.hpp"

static int	matchLocation(const std::string &uri, const std::string &path)
{
	if (uri.empty() || path.empty())
		return (0);
	if (uri.size() < path.size())
		return (0);
	if  (uri.compare(0, path.size(), path))
		return (0);
	if (uri.size() != path.size() && path[path.size() - 1] != '/'
			&& uri[path.size()] != '/')
		return (0);
	return 1;
}

const Location*	RequestHandler::getLocation()
{
	std::vector<Location>::const_iterator iter = _config.locations.begin();
	size_t	bestPathLocation = 0;
	const Location *bestLocation = NULL;

	while (iter != _config.locations.end())
	{
		if (matchLocation(_request.uri, iter->path)
		{
			if (bestLocation == NULL || iter->path.size() > bestPathLocation)
			{
				bestPathLocation = iter->path.size();
				bestLocation = &(*iter);
			}
		}
		iter++;
	}
	return (bestLocation);
}

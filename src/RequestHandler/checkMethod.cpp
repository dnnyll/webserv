#include "RequestHandler.hpp"

int RequestHandler::checkMethod()
{
	std::vector<std::string>const_iterator iter = _location->methods.begin(); 

	while (iter != _location->methods.begin())
	{
		if (*iter == _request.method)
			return (0);
	}
	return (1);
}

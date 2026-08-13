#include "Router.hpp"

static bool	isMethodAllowed(const std::string &method, const std::vector<std::string> 
		&methods)
{
	std::vector<std::string>::const_iterator	iter = methods.begin();

	while (iter != methods.end())
	{
		if (method == *iter)
			return true;
		iter++;
	}
	return false;
}

int Router::checkMethod()
{
	if (!isMethodAllowed(this->_request.method, this->_effconf.methods))
		return 1;
	return 0;
}

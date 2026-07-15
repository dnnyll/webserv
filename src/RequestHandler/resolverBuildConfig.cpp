#include "RequestHandler.hpp"

static std::string	chooseBestValue(const std::string &official, const std::string
						&fallback)
{
	if (official.empty())
		return fallback;
	return official;
}

//construction de la effconfig
void	RequestHandler::resolverBuildConfig()
{
	if (!this->_location)
	{
		this->_effconf.path = "/";
		this->_effconf.root = this->_config.root;
		this->_effconf.index = this->_config.index;
		this->_effconf.methods =  this->_config.methods;
		this->_effconf.autoindex = this->_config.autoindex;
		return ;
	}
	this->_effconf.path = this->_location->path;
	this->_effconf.root = chooseBestValue(this->_location->root, this->_config.root);
	this->_effconf.index = chooseBestValue(this->_location->index,
			this->_config.index);
	if (!this->_location->methods.empty())
		this->_effconf.methods = this->_location->methods;
	else
		this->_effconf.methods = this->_config.methods;
	this->_effconf.autoindex = this->_location->autoindex;
}

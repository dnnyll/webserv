#include "Router.hpp"

static std::string	chooseBestValue(const std::string &official, const std::string
						&fallback)
{
	if (official.empty())
		return fallback;
	return official;
}

//#include <iostream>
//#include <vector>
//
//static void printVector(const std::vector<std::string>& vec)
//{
//    std::vector<std::string>::const_iterator it;
//
//	std::cout << "PRINT VECTOR BUILD CONFIG" << std::endl;
//    for (it = vec.begin(); it != vec.end(); ++it)
//	{
//        std::cout << *it << std::endl;
//	}
//	std::cout << "PRINT VECTOR BUILD CONFIG" << std::endl;
//}

//construction de la effconfig
void	Router::resolveBuildConfig()
{
	if (!this->_location)
	{
		this->_effconf.client_max_body_size = this->_config.client_max_body_size;
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
	this->_effconf.upload_store = this->_location->upload_store;
	this->_effconf.client_max_body_size = this->_location->client_max_body_size;
	this->_effconf.cgi_pass = this->_location->cgi_pass;
	//TODO jules a supp ou non? debug?
	//printVector(this->_effconf.methods);
}

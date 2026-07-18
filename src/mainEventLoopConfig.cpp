#include    "../inc/EventLoop.hpp"
#include    "../inc/AcceptHandler.hpp"
#include    "../inc/Config.hpp"
#include	<iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return (1);
	}

	Config config;
	config.parse(argv[1]);

	EventLoop reactor;
	std::vector<AcceptHandler*> listeners;

	const std::vector<ServerBlock> &servers = config.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		AcceptHandler	*listener = new AcceptHandler(servers[i].port, servers[i].host, reactor);
		listeners.push_back(listener);
		reactor.addHandler(listener);
	}

	reactor.run();

	for (size_t i = 0; i < listeners.size(); i++)
		delete listeners[i];

	return (0);
}
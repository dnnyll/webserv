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

	const std::vector<ServerBlock> &servers = config.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		AcceptHandler *listener = new AcceptHandler(servers[i].port, servers[i].host, reactor);
		if (listener->getFd() < 0)
		{
			std::cerr << "Failed to setup listener for " << servers[i].host << ":" << servers[i].port << std::endl;
			delete listener;
			return (1);
		}
		reactor.addHandler(listener);
	}

	reactor.run();

	return (0);
}
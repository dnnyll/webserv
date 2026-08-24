#include	"../inc/EventLoop.hpp"
#include	"../inc/AcceptHandler.hpp"
#include	"../inc/Config.hpp"
#include	"../inc/check_path.hpp"
#include	<iostream>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return (1);
	}

	Config	config;

	try
	{
		check(argv[1]);
		config.parse(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return (1);
	}

	EventLoop	reactor;

	const std::vector<ServerBlock>	&servers = config.getServers();

	size_t	i = 0;

	while (i < servers.size())
	{
		AcceptHandler *listener = new AcceptHandler(servers[i], reactor);

		if (listener->getFd() < 0)
		{
			std::cerr << "[MAIN]Failed to setup listener for "
					<< servers[i].host << ":"
					<< servers[i].port << std::endl;
			delete listener;
			++i;
			continue ;
		}

		reactor.addHandler(listener);
		++i;
	}
	reactor.run();
	return (0);
}

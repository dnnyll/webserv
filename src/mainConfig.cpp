#include <iostream>
#include "../inc/check_path.hpp"
#include "../inc/Config.hpp"
#include "../inc/EventLoop.hpp"
#include "../inc/AcceptHandler.hpp"

int	main(int argc, char* argv[])
{
	/*{
		std::cout << "---------TEST CONFIG FILE---------" << std::endl;
		std::string path1 = "blabla.conf"; //right;
		std::string path2 = "../config_files/.conf"; //right;
		std::string path3 = "../config_files/.conf.conf"; //right
		std::string path4 = "../config_files/blabla.conf"; //right
		//std::string path5 = "../config_files/app_42.conf"; //right but not access
		//std::string path6 = "../config_files/conf."; //not right
		std::string path7 = "../config_files/empty.conf"; //empty

		try
		{
			std::cout << "Path 1: " << path1 << std::endl;
			check(path1);
			std::cout << "Path 2: " << path2 << std::endl;
			check(path2);
			std::cout << "Path 3: " << path3 << std::endl;
			check(path3);
			std::cout << "Path 4: " << path4 << std::endl;
			check(path4);
			//std::cout << "Path 5: " << path5 << std::endl;
			//check(path5);
			//std::cout << "Path 6: " << path6 << std::endl;
			//check(path6);
			//std::cout << "Path 7: " << path7 << std::endl;
			//check(path7);
		}
		catch(const std::exception &e)
		{
			std::cerr << "Exception caught: " << e.what() << std::endl;
		}
	}*/
	/*{
		try
		{
			Config config;
			config.parse("../config_files/server.conf");
		}
		catch(const std::exception& e)
		{
			std::cerr << "Config error: " << e.what() << '\n';
			return (1);
		}
	}*/
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return (1);
	}

	try
	{
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
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

}
#include <iostream>
#include "../inc/check_path.hpp"
#include "../inc/Config.hpp"
#include "../inc/event_loop.hpp"

int	main(void)
{
	{
		std::cout << "---------------- EVENT LOOP -------------------" << std::endl;
		try
		{
			Config	config;
			config.parse("config_files/server.conf");
			return event_loop(config);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		return (0);
	}

}
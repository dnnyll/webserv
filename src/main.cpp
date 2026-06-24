#include <iostream>
#include "../inc/check_path.hpp"
#include "../inc/event_loop.hpp"

int	main(void)
{
	{
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
	}
	{
		std::cout << "---------------- EVENT LOOP -------------------" << std::endl;
		return event_loop();
	}

}
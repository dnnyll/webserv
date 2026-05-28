#ifndef		SERVER_HPP
# define	SERVER_HPP

# include	<map>
# include	"ServerConfig.hpp"
# include	"Client.hpp"

class	Server
{
	public:
		Server();

	private:
		int						server_fd;
		std::map<int, Client>	clients;
		ServerBlock&			config;
};

#endif
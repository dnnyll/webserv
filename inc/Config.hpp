#ifndef		SERVERCONFIG_HPP
# define	SERVERCONFIG_HPP

#include	<string>
#include	<vector>

struct	Location
{
	std::string	path; //path ou se situe les regle
	std::string	root; //regle qui dit que ca se situe a cet endroit
	std::string	index;
};

struct	ServerBlock
{
	std::string	host;
	int			port;
	std::string	server_name;
	std::vector<Location>	locations;
	std::string	root;
	size_t		client_max_body_size;
};

class	Config
{
	public:
		Config();
		void parse(std::string filepath);
		const std::vector<ServerBlock>& getServers() const;
		
	private:
		std::vector<ServerBlock> servers;
};

#endif

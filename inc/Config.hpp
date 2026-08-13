#ifndef		SERVERCONFIG_HPP
# define	SERVERCONFIG_HPP

#include	<string>
#include	<vector>
#include	<map>

struct	Location
{
	std::string					path;
	std::string					root;
	std::string					index;
	std::vector<std::string>	methods;
	bool						autoindex;
	std::string					upload_store;
	std::map<std::string, std::string>	cgi_pass;
	int							redirect_code;
	std::string					redirect_url;
	size_t						client_max_body_size;
	Location() : autoindex(false), redirect_code(0), client_max_body_size(0) {}
};

struct	ServerBlock
{
	std::string					host;
	size_t						port;
	std::string					server_name;
	std::map<int, std::string>	error_pages;
	std::vector<Location>		locations;
	std::string					root;
	std::string					index;
	std::vector<std::string>	methods;
	size_t						client_max_body_size;
	bool						autoindex;

	ServerBlock() : port(0), client_max_body_size(1000000), autoindex(false) {}
};

class	Config
{
	public:
		void parse(const std::string &filepath);
		const std::vector<ServerBlock>& getServers() const;

		class ConfigException : public std::exception
		{
			public:
				ConfigException(const std::string &msg) : _msg(msg) {}
				virtual ~ConfigException() throw() {}
				virtual const char* what() const throw() { return _msg.c_str(); }
			private:
				std::string	_msg;
		};
		
	private:
		std::vector<ServerBlock> _servers;
};

#endif

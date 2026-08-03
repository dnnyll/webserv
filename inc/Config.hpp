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
	//TODO (jules) transformer en map<std::string, std::string> cgi extension/cgi path
	std::string					cgi_extension;
	std::string					cgi_path;
	std::string					redirect;
	//size_t						client_max_body_size;
	Location() : autoindex(false) {}
};

//	TODO (danny or jules): manage client_max_body_size

// Option A — Check in HttpRequest:

// Reject oversized bodies during parsing, before Router is even involved
// Advantage: stops reading data early, saves bandwidth
// Disadvantage: requires passing config into parsing layer

// Option B — Check in Router (your teammate's responsibility):

// Parser reads everything, Router checks size and returns 413
// Advantage: clean separation — parsing doesn't know about config
// Disadvantage: you've already read all the data before rejecting

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

#include "../../inc/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

static void expect_value(const std::vector<std::string> &tokens, size_t i, const std::string &key)
{
	if (i >= tokens.size())
		throw Config::ConfigException("Missing value after '" + key + "'");
}

static bool is_number(const std::string &s)
{
	if (s.empty())
		return (false);
	size_t start = (s[0] == '-') ? 1 : 0;
	if (start == s.size())
		return (false);
	for (size_t k = start; k < s.size(); ++k)
		if (!std::isdigit(static_cast<unsigned char>(s[k])))
			return (false);
	return (true);
}

static bool is_valid_ip(const std::string &ip)
{
	if (ip == "localhost")
		return (true);
	
	int dots = 0;
	size_t start = 0;

	for(size_t pos = 0; pos <= ip.size(); ++pos)
	{
		if (pos == ip.size() || ip[pos] == '.')
		{
			if (pos == start)
				return (false);
			
			std::string segment = ip.substr(start, pos - start);
			if (segment.size() > 3)
				return false;
			for (size_t k = 0; k < segment.size(); ++k)
				if (!std::isdigit(static_cast<unsigned char>(segment[k])))
					return (false);

			int value = std::atoi(segment.c_str());
			if (value < 0 || value > 255)
				return (false);

			if (pos != ip.size())
				dots++;
			start = pos + 1;
		}
	}
	return dots == 3;
}

static Location parse_location(const std::vector<std::string> &tokens, size_t &i)
{
	Location loc;

	expect_value(tokens, i, "location");

	if (tokens[i].empty())
		throw Config::ConfigException("Location path can't ne empty");

	loc.path = tokens[i++];
	expect_value(tokens, i, "location path");

	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after location path");
	
	bool root_set = false;
	bool index_set = false;
	bool autoindex_set = false;
	bool upload_store_set = false;
	bool redirect_set = false;
	bool methods_set = false;
	bool client_max_body_size_set = false;
	
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string key = tokens[i++];

		if (key == "root")
		{
			expect_value(tokens, i, key);
			if (root_set)
				throw Config::ConfigException("Duplicate 'root' directive in location " + loc.path);
			if (tokens[i].empty())
				throw Config::ConfigException("'root' can't be empty in location " + loc.path);
			loc.root = tokens[i++];
			root_set = true;
		}
		else if (key == "index")
		{
			expect_value(tokens, i, key);
			if (index_set)
				throw Config::ConfigException("Duplicate 'index' directive in location " + loc.path);
			loc.index = tokens[i++];
			index_set = true;
		}
		else if (key == "autoindex")
		{
			expect_value(tokens, i, key);
			if (autoindex_set)
				throw Config::ConfigException("Duplicate 'root' directive in location " + loc.path);
			if (tokens[i] != "on" && tokens[i] != "off")
				throw Config::ConfigException("'autoindex' must be 'on' or 'off' in location " + loc.path + ", got: " + tokens[i]);
			loc.autoindex = (tokens[i++] == "on");
			autoindex_set = true;
		}
		else if (key == "upload_store")
		{
			expect_value(tokens, i, key);
			if (upload_store_set)
				throw Config::ConfigException("Duplicate 'upload_store' directive in location " + loc.path);
			loc.upload_store = tokens[i++];
			upload_store_set = true;
		}
		else if (key == "return")
		{
			if (redirect_set)
				throw Config::ConfigException("Duplicate 'return' directive in location " + loc.path);

			expect_value(tokens, i, "return code");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid return code: " + tokens[i]);
			int code = std::atoi(tokens[i++].c_str());
			if (code < 300 || code > 399)
				throw Config::ConfigException("return code muste be a valid redirect code (300-399)");
			
			expect_value(tokens, i, "return url");
			if (tokens[i].empty() || tokens[i] == ";")
				throw Config::ConfigException("return url can't be empty in location " + loc.path);
			
			loc.redirect_url = tokens[i++];
			loc.redirect_code = code;
			redirect_set = true;
		}
		else if (key == "cgi_pass")
		{
			expect_value(tokens, i, "cgi_pass extension");
			std::string ext = tokens[i++];
			expect_value(tokens, i, "cgi_pass path");
			std::string path = tokens[i++];

			if (loc.cgi_pass.count(ext))
				throw Config::ConfigException("Duplicate 'cgi_pass' for extension " + ext + " in location " + loc.path);
			
			loc.cgi_pass[ext] = path;
		}
		else if (key == "methods")
		{
			if (methods_set)
				throw Config::ConfigException("Duplicate 'methods' directive in location " + loc.path);
			while (i < tokens.size() && tokens[i] != ";")
			{
				const std::string &m = tokens[i];
				if (m != "GET" && m != "POST" && m != "DELETE")
					throw Config::ConfigException("Unsupported HTTP method: " + m);
				loc.methods.push_back(tokens[i++]);
			}
			if (loc.methods.empty())
				throw Config::ConfigException("'methods' directive can't be empty in location " + loc.path);
			methods_set = true;
		}
		else if (key == "client_max_body_size")
		{
			expect_value(tokens, i, key);
			if (client_max_body_size_set)
				throw Config::ConfigException("Duplicate 'client_max_body_size' directive");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid client_max_body_size value");
			loc.client_max_body_size = std::atoi(tokens[i++].c_str());
			client_max_body_size_set = true;
		}
		else
			throw Config::ConfigException("Unknown location directive: " + key);

		if (i >= tokens.size() || tokens[i] != ";")
			throw Config::ConfigException("Expected ';' after directive '" + key + "'");
		i++;
	}
	if (i >= tokens.size() || tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close location block");
	i++;
	return loc;
}

static ServerBlock parse_server(const std::vector<std::string> &tokens, size_t &i)
{
	ServerBlock server;

	bool listen_set = false;
	bool host_set = false;
	bool root_set = false;
	bool client_max_body_size_set = false;

	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after server");
	

	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string key = tokens[i++];

		if (key == "listen")
		{
			expect_value(tokens, i, key);
			if (listen_set)
				throw Config::ConfigException("Duplicate 'listen' directive");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid port value: " + tokens[i]);
			server.port = std::atoi(tokens[i++].c_str());
			listen_set = true;
		}
		else if (key == "host")
		{
			expect_value(tokens, i, key);
			if (host_set)
				throw Config::ConfigException("Duplicate 'host' directive");
			if (!is_valid_ip(tokens[i]))
				throw Config::ConfigException("Invalid host/IP: " + tokens[i]);
			server.host = tokens[i++];
			host_set = true;
		}
		else if (key == "server_name")
		{
			expect_value(tokens, i, key);
			server.server_name = tokens[i++];
		}
		else if (key == "root")
		{
			expect_value(tokens, i, key);
			if (root_set)
				throw Config::ConfigException("Duplicate 'root' directive");
			if (tokens[i].empty())
				throw Config::ConfigException("'root' can't be empty");
			server.root = tokens[i++];
			root_set = true;
		}
		else if (key == "client_max_body_size")
		{
			expect_value(tokens, i, key);
			if (client_max_body_size_set)
				throw Config::ConfigException("Duplicate 'client_max_body_size' directive");
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid client_max_body_size value");
			server.client_max_body_size = std::atoi(tokens[i++].c_str());
			client_max_body_size_set = true;
		}
		else if (key == "error_page")
		{
			expect_value(tokens, i, key);
			if (!is_number(tokens[i]))
				throw Config::ConfigException("Invalid error_page code: " + tokens[i]);
			int code = std::atoi(tokens[i++].c_str());
			if (code < 100 || code > 599)
				throw Config::ConfigException("error_page code must be between 100 and 599");
			expect_value(tokens, i, "error_page");
			if (tokens[i].empty())
				throw Config::ConfigException("error_page path can't be empty");
			server.error_pages[code] = tokens[i++];
		}
		else if (key == "location")
		{
			server.locations.push_back(parse_location(tokens, i));
			continue;
		}
		else
			throw Config::ConfigException("Unknown server directive : " + key);

		if (i >= tokens.size() || tokens[i] != ";")
			throw Config::ConfigException("Expected ';' after directive '" + key + "'");
		i++;
	}
	if (i >= tokens.size() || tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close server block");

	i++;

	//check all value
	if (!listen_set)
		throw Config::ConfigException("Server block missing 'listen' directive");

	if (server.port < 1 || server.port > 65535)
		throw Config::ConfigException("The server " + server.server_name + " has a wrong port number, must be between 1 and 65535");

	if (server.locations.empty())
		throw Config::ConfigException("Server block must define at least one 'location'");

	if (!host_set)
		server.host = "0.0.0.0";

	return server;
}

static std::vector<std::string> tokenize(const std::string &filepath)
{
	std::ifstream file(filepath.c_str());
	if (!file.is_open())
		throw Config::ConfigException("Cannot open file: " + filepath);
	
	std::vector<std::string>	tokens;
	std::string 				line;

	while (std::getline(file, line))
	{
		size_t	comment = line.find('#');
		if (comment != std::string::npos)
			line = line.substr(0, comment);

		std::istringstream	iss(line);
		std::string			word;
		while (iss >> word)
		{
			for (size_t i=0; i < word.size(); i++)
			{
				if (word[i] == '{' || word[i] == '}' || word[i] == ';')
				{
					if (i > 0)
						tokens.push_back(word.substr(0, i));
					tokens.push_back(std::string(1, word[i]));
					word = word.substr(i + 1);
					i = -1;
				}
			}
			if (!word.empty())
				tokens.push_back(word);
		}
	}
	return tokens;
}

static void validate_inheritance(std::vector<ServerBlock> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		ServerBlock &server = servers[i];

		for(size_t j = 0; j < server.locations.size(); ++j)
		{
			Location &loc = server.locations[j];

			if (loc.root.empty() && server.root.empty())
				throw Config::ConfigException("No 'root' defined for location \"" + loc.path + "\" and no server-level fallback");

			if (loc.client_max_body_size == 0)
				loc.client_max_body_size = server.client_max_body_size;
		}
	}
}

static void validate_no_duplicate_servers(const std::vector<ServerBlock> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		for (size_t j = i + 1; j < servers.size(); ++j)
		{
			const ServerBlock &a = servers[i];
			const ServerBlock &b = servers[j];

			if (a.host == b.host && a.port == b.port && a.server_name == b.server_name)
				throw Config::ConfigException ("Duplicate server is no possible");
		}
	}
}

void Config::parse(const std::string &filepath)
{
	std::vector<std::string> tokens = tokenize(filepath);
	size_t i = 0;

	while (i < tokens.size())
	{
		if (tokens[i] == "server")
		{
			i++;
			_servers.push_back(parse_server(tokens, i));
		}
		else
			throw Config::ConfigException("Expected 'server' block, got : " + tokens[i]);
	}

	validate_inheritance(_servers);
	validate_no_duplicate_servers(_servers);

	for (size_t s = 0; s < _servers.size(); s++)
	{
		std::cout << "Server: " << _servers[s].host << ":" << _servers[s].port << std::endl;
		for (size_t l = 0; l < _servers[s].locations.size(); l++)
			std::cout << "  Location: " << _servers[s].locations[l].path << std::endl;
	}
}

const std::vector<ServerBlock>& Config::getServers() const
{
	return _servers;
}

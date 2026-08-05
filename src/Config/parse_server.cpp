#include <vector>
#include <string>
#include "../inc/config_utils.hpp"
#include "../inc/Config.hpp"
#include "../inc/parse_location.hpp"
#include "../inc/parse_server.hpp"

ServerBlock parse_server(const std::vector<std::string> &tokens, size_t &i)
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
			server.locations.push_back(parse_location(tokens, i));
		else
			throw Config::ConfigException("Unknown server directive : " + key);

		if (i < tokens.size() && tokens[i] == ";")
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
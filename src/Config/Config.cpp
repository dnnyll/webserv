#include "../inc/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

static Location parse_location(const std::vector<std::string> &tokens, size_t &i)
{
	Location loc;
	loc.path = tokens[i++]; //le path vient juste après "location"

	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after location path");
	
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string key = tokens[i++];

		if (key == "root")
			loc.root = tokens[i++];
		else if (key == "index")
			loc.index = tokens[i++];
		else if (key == "autoindex")
			loc.autoindex = (tokens[i++] == "on");
		else if (key == "upload_store")
			loc.upload_store = tokens[i++];
		else if (key == "redirect")
			loc.redirect = tokens[i++];
		else if (key == "cgi_pass")
		{
			loc.cgi_extension	= tokens[i++];
			loc.cgi_path		= tokens[i++];
		}
		else if (key == "methods")
		{
			while (i < tokens.size() && tokens[i] != ";")
				loc.methods.push_back(tokens[i++]);
		}
		else
			throw Config::ConfigException("Unknown location directive: " + key);
	
		if (tokens[i] == ";")
			i++;
	}
	if (tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close location block");
	i++;
	return loc;
}

static void check_config_value(ServerBlock server)
{
	if (server.port < 1 || server.port > 65535)
		throw Config::ConfigException("The server " + server.server_name + " has a wrong port number, must be between 1 and 65535");
	//if (server.host) check host is a IP adresse or localhost
	if (server.root.empty())
		throw Config::ConfigException("Server block missing 'root' directive");
	if (server.client_max_body_size < 0)
		throw Config::ConfigException("client_max_body_size cannot be negative");
	if (server.)
}

static ServerBlock parse_server(const std::vector<std::string> &tokens, size_t &i)
{
	ServerBlock server;

	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after server");
	
	while (i < tokens.size() && tokens[i] != "}")
	{
		std::string key = tokens[i++];

		if (key == "listen")
			server.port = std::atoi(tokens[i++].c_str());
		else if (key == "host")
			server.host = tokens[i++];
		else if (key == "server_name")
			server.server_name = tokens[i++];
		else if (key == "root")
			server.root = tokens[i++];
		else if (key == "client_max_body_size")
			server.client_max_body_size = std::atoi(tokens[i++].c_str());
		else if (key == "error_page")
		{
			int code = std::atoi(tokens[i++].c_str());
			server.error_pages[code] = tokens[i++];
		}
		else if (key == "location")
			server.locations.push_back(parse_location(tokens, i));
		else
			throw Config::ConfigException("Unknown server directive : " + key);

		if (i < tokens.size() && tokens[i] == ";")
			i++;
	}
	if (tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close server block");

	i++;

	//check all value
	check_config_value(server);

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
		//suppression des commentaires
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

//a ajouter, check si deux serveur ont le meme host, port, name -> exception !!
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

//port valable entre 1 et 65535
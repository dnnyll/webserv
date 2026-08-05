#include "../inc/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "../inc/parse_server.hpp"
#include "../inc/parse_location.hpp"

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

static void validate_root_inheritance(const std::vector<ServerBlock> &servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const ServerBlock &server = servers[i];

		for(size_t j = 0; j < server.locations.size(); ++j)
		{
			const Location &loc = server.locations[j];

			if (loc.root.empty() && server.root.empty())
			{
				throw Config::ConfigException("No 'root' defined for location \"" + loc.path + "\" and no server-level fallback");
			}
		}
	}
}

//a ajouter, check si deux serveur ont le meme host, port, name -> exception !!
void Config::parse(const std::string &filepath)
{
	std::vector<std::string> tokens = tokenize(filepath);
	size_t i = 0;

	std::cout << "DEBUG : start global parsing" << std::endl;

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

	validate_root_inheritance(_servers);

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

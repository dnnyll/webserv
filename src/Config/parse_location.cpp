#include <vector>
#include <string>
#include "../inc/config_utils.hpp"
#include "../inc/Config.hpp"

Location parse_location(const std::vector<std::string> &tokens, size_t &i)
{
	Location loc;

	expect_value(tokens, i, "location");
	if (tokens[i].empty())
		throw Config::ConfigException("Location path can't ne empty");
	loc.path = tokens[i++]; //le path vient juste après "location"

	expect_value(tokens, i, "location path");
	if (tokens[i++] != "{")
		throw Config::ConfigException("Expected '{' after location path");
	
	bool root_set = false;
	bool index_set = false;
	bool autoindex_set = false;
	bool upload_store_set = false;
	bool redirect_set = false;
	bool cgi_pass_set = false;
	bool methods_set = false;
	
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
		else if (key == "redirect")
		{
			expect_value(tokens, i, key);
			if (redirect_set)
				throw Config::ConfigException("Duplicate 'redirect' directive in location " + loc.path);
			loc.redirect = tokens[i++];
			redirect_set = true;
		}
		else if (key == "cgi_pass")
		{
			if (cgi_pass_set)
				throw Config::ConfigException("Duplicate 'cgi_pass' directive in location " + loc.path);
			expect_value(tokens, i, "cgi_pass extension");
			loc.cgi_extension = tokens[i++];
			expect_value(tokens, i, "cgi_pass path");
			loc.cgi_path		= tokens[i++];
			cgi_pass_set = true;
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
		else
			throw Config::ConfigException("Unknown location directive: " + key);
	
		if (i < tokens.size() && tokens[i] == ";")
			i++;
	}
	if (i >= tokens.size() || tokens[i] != "}")
		throw Config::ConfigException("Expected '}' to close location block");
	i++;
	return loc;
}
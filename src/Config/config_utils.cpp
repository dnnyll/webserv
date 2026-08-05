#include <vector>
#include <string>
#include <exception>
#include "../inc/Config.hpp"

void expect_value(const std::vector<std::string> &tokens, size_t i, const std::string &key)
{
	if (i >= tokens.size())
		throw Config::ConfigException("Missing value after '" + key + "'");
}

bool is_number(const std::string &s)
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

bool is_valid_ip(const std::string &ip)
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
				return (false); //segment vide
			
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
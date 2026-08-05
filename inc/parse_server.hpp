#ifndef PARSE_SERVER_HPP
# define PARSE_SERVER_HPP

# include <vector>
# include <string>
# include "../inc/Config.hpp"

ServerBlock parse_server(const std::vector<std::string> &tokens, size_t &i);

#endif
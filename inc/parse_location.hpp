#ifndef PARSE_LOCATION_HPP
# define PARSE_LOCATION_HPP

# include <string>
# include <vector>
# include "../inc/Config.hpp"

Location parse_location(const std::vector<std::string> &tokens, size_t &i);

#endif
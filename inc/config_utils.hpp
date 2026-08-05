#ifndef CONFIG_UTILS_HPP
# define CONFIG_UTILS_HPP

# include <vector>
# include <string>

void expect_value(const std::vector<std::string> &tokens, size_t i, const std::string &key);
bool is_number(const std::string &s);
bool is_valid_ip(const std::string &ip);

#endif
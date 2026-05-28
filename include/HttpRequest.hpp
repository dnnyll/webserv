#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <map>

class HttpRequest
{
	public:
		HttpRequest();

	private:
		std::string	method;
		std::string	uri;
		std::map<std::string, std::string> headers;
		std::string	body;
};

#endif
#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HTTP

# include <string>
# include <map>

class HttpResponse
{
	public:
		HttpResponse();

	private:
		int									status;
		std::map<std::string, std::string>	headers;
		std::string							body;
};

#endif
#include	"../inc/HttpMessage.hpp"
#include	"../inc/HttpRequest.hpp"
#include	<string>

int	main()
{
	HttpRequest	test1;

	std::string raw = "GET /index.html HTTP/1.1\r\n"
				"Host: localhost:8080\r\n"
				"Content-Length: 0\r\n"
				"\r\n";

	test1.getData(raw);

	return (0);
}

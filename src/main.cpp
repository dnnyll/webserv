#include "RequestHandler.hpp"

int main(void)
{
	ServerBlock c;
	HttpRequest r;
	//hardcode config + request

	RequestHandler a(r, c);

	a.process();
	std::cout << "ok im working" << std::endl;
	return (1);
}


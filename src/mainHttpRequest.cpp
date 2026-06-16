#include	"../inc/HttpMessage.hpp"
#include	"../inc/HttpRequest.hpp"
#include	<string>

int	main()
{
	HttpRequest test1;

	std::string chunks[] =
	{
		//	regular test
		// "GE",
		// "T /in",
		// "dex.html ",
		// "HTTP/1.1\r",
		// "\nHost: lo",
		// "calhost:80",
		// "80\r\nCont",
		// "ent-Length",
		// ": 28\r\n",
		// "\r",
		// "\n"
		// "\nhe",
		// "llo mate! h",
		// "ow's it go",
		// "ing? a",
		// "re you dea",
		// "d yet?????"

		//	chunked test
		"POST /upl",
		"oad HTTP/1.1\r\n",
		"Host: local",
		"host:8080\r\n",
		"Transfer-Enc",
		"oding: chunked\r\n",
		"\r\n",
		"5\r\n",
		"he",
		"llo\r\n",
		"6\r\n wo",
		"rld\r\n",
		"0\r",
		"\n",
		"\r\n"

		// "GE",
		// "T ",
		// std::string(100, ' '),
		// "/index.h",
		// "tml H",
		// "TTP/1.1\r\n"
	};

	for (size_t i = 0; i < sizeof(chunks) / sizeof(chunks[0]); ++i)
	{
		std::cout << "\n--- Sending chunk #" << i << " ---" << std::endl;
		std::cout << "[" << chunks[i] << "]" << std::endl;

		test1.getData(chunks[i]);
	}

	return (0);
}

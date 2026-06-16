#include	"../inc/ClientHandler.hpp"
#include	<string>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>

int	main()
{
	int			listenFd = socket(AF_INET, SOCK_STREAM, 0);
	int			opt = 1;
	
	setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in		addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);

	bind(listenFd, (sockaddr*)&addr, sizeof(addr));
	listen(listenFd, 10);

	std::cout << "listening on 8080..." << std::endl;

	int				clientFd = accept(listenFd, NULL, NULL);	// blocking
	ClientHandler	handler(clientFd);

	while (true)
	{
		handler.handleRead();						// call repeatedly, see it parse + respond
		handler.handleWrite();
	}
	close(listenFd);
	return (0);
}

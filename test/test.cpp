//	this is a poll test

//#include	<poll.h>
#include	<iostream>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include 	<arpa/inet.h>
#include 	<netinet/in.h>


#define BUFFER_SIZE 50

int	main(void)
{
	struct sockaddr_in	addr;

	memset(&addr, 0, sizeof(addr));   // important

	addr.sin_family = AF_INET;        // IPv4
	addr.sin_port = htons(8080);      // port (conversion réseau)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP

	//poll();
	int socket_fd = socket(AF_INET ,SOCK_STREAM, 0);
	bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
	listen(socket_fd, 10);

	
	while(1)
	{
		int client_fd = accept(socket_fd, NULL, NULL);
		char buffer[BUFFER_SIZE] = {};
		int readed = read(client_fd, buffer, sizeof(buffer));
		std::cout << readed << std::endl;
		write(0, buffer, sizeof(buffer));
	}
	return (0);
}
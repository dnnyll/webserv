
#include	"../inc/AcceptHandler.hpp"
#include	"../inc/ClientHandler.hpp"
#include	<iostream>
#include	<sys/socket.h>
#include	<fcntl.h>
#include	<netinet/in.h>
#include	<unistd.h>

/*
** Create a listening socket handler.
**
** Stores a reference to the EventLoop and initializes the
** server socket by calling setupSocket().
**
** After construction, the socket is bound to the requested
** port and ready to accept incoming connections.
*/
AcceptHandler::AcceptHandler(int port, EventLoop &reactor) : _reactor(reactor)
{
	setupSocket(port);
}

/*
** Cleanup for the listening socket.
**
** Closes the server socket and stops accepting new
** client connections.
*/
AcceptHandler::~AcceptHandler()
{
	close(_fd);
}

/*
** Return the listening socket file descriptor.
**
** The EventLoop uses this fd when building its poll()
** set so it can detect when new connections are waiting
** to be accepted.
*/
int	AcceptHandler::getFd() const
{
	return (_fd);
}

/*
** Listening sockets are permanent handlers.
**
** Returns false because the server socket should remain
** active for the lifetime of the EventLoop.
**
** Client connections may be removed, but the listening
** socket stays registered so new clients can connect.
*/
bool	AcceptHandler::setClose() const
{
	return (false);
}

/*
** Accept a pending client connection.
**
** When poll() marks the listening socket as readable,
** it means at least one client is waiting in the kernel's
** connection queue.
**
** accept() removes one connection from that queue and
** creates a new connected socket dedicated to that client.
**
** A ClientHandler is then created to manage the connection,
** and registered with the EventLoop so future read/write
** events for that client can be handled independently.
**
** This function never processes HTTP requests directly.
** Its only job is to create ClientHandlers.
*/
void	AcceptHandler::handleRead()
{
	int	clientFd = accept(_fd, NULL, NULL);

	if (clientFd < 0)
		return ; // no connection actually pending, or error — ignore

	ClientHandler *client = new ClientHandler(clientFd);
	_reactor.addHandler(client);
}

/*
** Listening sockets never send data.
**
** The server socket only accepts incoming connections.
** All communication with clients is performed through
** ClientHandler instances.
*/
void	AcceptHandler::handleWrite()
{
}

/*
** Create and initialize the listening socket.
**
** Steps:
**
** 1. Create a TCP socket.
** 2. Enable SO_REUSEADDR so the port can be reused
**    immediately after restarting the server.
** 3. Bind the socket to the specified port.
** 4. Put the socket into listening mode.
**
** After setup completes, incoming connection requests
** are queued by the kernel until handleRead() accepts
** them.
*/
void	AcceptHandler::setupSocket(int port)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);

	int	opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in	addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	bind(_fd, (sockaddr*)&addr, sizeof(addr));
	listen(_fd, 10);

	std::cout << "listening on " << port << "..." << std::endl;
}
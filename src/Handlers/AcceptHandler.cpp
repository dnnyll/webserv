
#include	"../inc/AcceptHandler.hpp"
#include	"../inc/ClientHandler.hpp"
#include	"../inc/EventHandler.hpp"
#include	"../inc/Config.hpp"
#include	<iostream>
#include	<sys/socket.h>
#include	<fcntl.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<arpa/inet.h>
#include	<cerrno>
#include	<cstring>

/*
** Create a listening socket handler.
**
** Stores a reference to the EventLoop and initializes the
** server socket by calling setupSocket().
**
** After construction, the socket is bound to the requested
** port and ready to accept incoming connections.
*/
AcceptHandler::AcceptHandler(const ServerBlock &block, EventLoop &reactor) : _reactor(reactor), _serverBlock(block)
{
	setupSocket(block.port, block.host);
	std::cout << "[ACCEPTHANDLER] _fd = " << _fd << std::endl;
}

/*
	Cleanup for the listening socket.
	Closes the server socket and stops accepting new client connections.
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
bool	AcceptHandler::isClosed() const
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
	{
		std::cout << "[ACCEPTHANDLER] accept() failed" << std::endl;
		return ;
	}

 	fcntl(clientFd, F_SETFL, O_NONBLOCK); //	sets client socket non-blocking

	std::cout << "[ACCEPTHANDLER] new client connected, fd=" << clientFd << std::endl;

	ClientHandler *client = new ClientHandler(clientFd, _serverBlock);	_reactor.addHandler(client);

	std::cout << "[ACCEPTHANDLER] ClientHandler created and registered" << std::endl;
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
void AcceptHandler::setupSocket(int port, const std::string &host)
{
	//	TODO: danny+alexis: fix config of ports outside the range
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Invalid port: " << port << std::endl;
		_fd = -1;
		return ;
	}

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
	{
		std::cerr << "socket() failed: " << strerror(errno) << std::endl;
		return ;
	}

	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	if (host.empty() || host == "0.0.0.0")
		addr.sin_addr.s_addr = INADDR_ANY;
	else
		addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port = htons(port);

	if (bind(_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "bind() failed: " << strerror(errno) << std::endl;
		return ;
	}

	if (listen(_fd, 10) < 0)
	{
		std::cerr << "listen() failed: " << strerror(errno) << std::endl;
		return ;
	}

	std::cout << "listening on " << host << ":" << port << "..." << std::endl;
}

/*
	** AcceptHandler never writes to its fd.
	** The listening socket only receives incoming connections,
	** it never sends data directly.
	** Always returns false.
*/
bool	AcceptHandler::isWritable() const
{
	return (false);
}

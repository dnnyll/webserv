
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

AcceptHandler::AcceptHandler(const ServerBlock &block, EventLoop &reactor) : _reactor(reactor), _serverBlock(block)
{
	setupSocket(block.port, block.host);
	std::cout << "[ACCEPTHANDLER] _fd = " << _fd << std::endl;
}

AcceptHandler::~AcceptHandler()
{
	close(_fd);
}

int	AcceptHandler::getFd() const
{
	return (_fd);
}

bool	AcceptHandler::getClosed() const
{
	return (false);
}

void	AcceptHandler::handleRead()
{
	int	clientFd = accept(_fd, NULL, NULL);

	if (clientFd < 0)
	{
		std::cout << "[HANDLEREAD] accept() failed" << std::endl;
		return ;
	}

 	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	std::cout << "[HANDLEREAD] new client connected, fd=" << clientFd << std::endl;

	ClientHandler *client = new ClientHandler(clientFd, _serverBlock, _reactor);	_reactor.addHandler(client);

	std::cout << "[HANDLEREAD] ClientHandler created and registered" << std::endl;
}

void	AcceptHandler::handleWrite()
{
}

void AcceptHandler::setupSocket(int port, const std::string &host)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_fd < 0)
	{
		std::cerr << "[SETUPSOCKET]Error: socket() failed: " << strerror(errno) << std::endl;
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
		std::cerr << "[SETUPSOCKET]Bind() failed: " << strerror(errno) << std::endl;
		return ;
	}

	if (listen(_fd, 10) < 0)
	{
		std::cerr << "[SETUPSOCKET]Listen() failed: " << strerror(errno) << std::endl;
		return ;
	}

	std::cout << "[SETUPSOCKET]Listening on " << host << ":" << port << "..." << std::endl;
}

bool	AcceptHandler::isWritable() const
{
	return (false);
}

void	AcceptHandler::setClosed()
{
}
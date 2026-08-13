#ifndef		ACCEPTHANDLER_HPP
# define	ACCEPTHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	"../inc/EventLoop.hpp"
#include	"../inc/Config.hpp"
#include	<string>

/*
** AcceptHandler
**
** Represents the server's listening socket.
**
** Responsibilities:
** - create and configure the listening socket
** - accept incoming TCP connections
** - create a ClientHandler for each new client
** - register new ClientHandlers with the EventLoop
**
** Only one AcceptHandler normally exists per listening port.
**
** Event flow:
**
**   Client connects
**          │
**          ▼
**   listening socket becomes readable
**          │
**          ▼
**   AcceptHandler::handleRead()
**          │
**          ▼
**   accept()
**          │
**          ▼
**   new ClientHandler(clientFd)
**          │
**          ▼
**   EventLoop.addHandler()
*/

class	AcceptHandler : public	EventHandler
{
	private:
		int			_fd;
		EventLoop	&_reactor;
		ServerBlock _serverBlock;

	//	methods
	public:
		AcceptHandler(const ServerBlock &block, EventLoop &reactor);
		~AcceptHandler();

		//	virtual overwritten methods
		void	handleRead();
		void	handleWrite();
		int		getFd() const;
		bool	getClosed() const;
		void	setClosed();
		bool	isWritable() const;

	private:
		void	setupSocket(int port, const std::string &host);
};

#endif

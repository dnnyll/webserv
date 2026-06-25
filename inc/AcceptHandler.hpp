#ifndef		ACCEPTHANDLER_HPP
# define	ACCEPTHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	"../inc/EventLoop.hpp"

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
	public:
		AcceptHandler(int port, EventLoop &reactor);
		//means "a reference to an EventLoop, and I'll refer to it as reactor inside this class." 
		//The & binds to the type, not the variable name.
		~AcceptHandler();
		//	virtual overwritten methods
		void	handleRead();
		void	handleWrite();
		int		getFd() const;
		bool	isClosed() const;
		bool	isWritable() const;

	private:
		int		_fd;
		EventLoop  &_reactor;

	//	methods
	void	setupSocket(int port);
};

#endif
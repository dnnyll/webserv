#ifndef		CLIENTHANDLER_HPP
# define	CLIENTHANDLER_HPP

#include	"../inc/HttpRequest.hpp"
#include	"../inc/EventHandler.hpp"

/*
** ClientHandler
**
** Represents one connected client.
**
** Responsibilities:
** - receive bytes from the socket
** - feed data into the HTTP request parser
** - detect when a request is complete
** - store serialized HTTP responses
** - send responses back to the client
** - manage connection persistence (keep-alive)
**
** One ClientHandler instance exists per connected client socket.
*/
class	ClientHandler : public	EventHandler
{
	public:
		ClientHandler(int fd);
		~ClientHandler();

		void	handleRead();
		void	handleWrite();
		int		getFd() const;

	private:
		int				_fd;			//	the client's socket fd
										//	needed for recv()/send()/close(), and to implement getFd()
		HttpRequest		_request;		//	accumulates incoming bytes, parses them
										//	holds the parsing state machine across multiple handleRead() calls
		std::string		_outBuffer;		//	bytes waiting to be sent back
										//	send() might not send everything in one call (non-blocking socket),
										//	so you need to remember what's left to send
		bool			_keepAlive;		//	should connection stay open after response?
										//	HTTP/1.1 defaults to keep-alive; after a response is fully sent, 
										//	you don't close the fd, 
										//	you reset _request and wait for the next request
		bool			_setClose;		//	private data, ClintHandler only

		//	methods
		bool	setClose() const;
};

#endif

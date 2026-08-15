#ifndef		CLIENTHANDLER_HPP
# define	CLIENTHANDLER_HPP

#include	"../inc/HttpRequest.hpp"
#include	"../inc/EventHandler.hpp"
#include	"../inc/Config.hpp"
#include	"../inc/EventLoop.hpp"
#include	<ctime>

struct	CgiAlive;

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
** - own a shared "am I still alive" flag consumed by any in-flight
**   CgiReadHandler, so a client disconnecting mid-CGI doesn't leave
**   the CGI side writing into freed memory
**
** One ClientHandler instance exists per connected client socket.
*/
class	ClientHandler : public	EventHandler
{
	public:
		ClientHandler(int fd, const ServerBlock &Block, EventLoop &reactor);
		~ClientHandler();

		void	handleRead();
		void	handleWrite();
		int		getFd() const;
		bool	isWritable() const;
		private:

		void	processRequest();
		
		int					_fd;
		const ServerBlock	&_config;
		EventLoop 			&_reactor;
		HttpRequest			_request;
		std::string			_outBuffer;
		bool				_keepAlive;
		bool				_getClosed;
		bool				_readClosed;
		CgiAlive			*_clientAlive;
		time_t				_lastActivity;

		bool	getClosed() const;
		void	setClosed();
		bool	shouldTimeout() const;
		void	onTimeout();
};

#endif

#ifndef		CLIENTHANDLER_HPP
# define	CLIENTHANDLER_HPP

#include	"../inc/HttpRequest.hpp"
#include	"../inc/EventHandler.hpp"

class	ClientHandler : public	EventHandler
{
	public:
		ClientHandler(int fd);
		~ClientHandler();

		void	handleRead();

		/*
		recv() fills buf[4096]	(temporary, local)
				│
				▼
		_request.getData(buf)	(appends to _buffer, persistent)
				│
				▼
		_buffer parsed by decode() state machine
		*/

		/*
		Client (browser/curl)
			│
			│	sends raw HTTP request bytes over TCP
			▼
		OS kernel
		(socket buffer for fd _fd)
			│
			│	recv(_fd, buf, sizeof(buf), 0)
			▼
		your buf[4096] array
			│
			│	_request.getData(std::string(buf, n))
			▼
		HttpRequest._buffer → parsed into method/uri/headers/body
		*/
		
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
		bool			_setClose;
	};

#endif

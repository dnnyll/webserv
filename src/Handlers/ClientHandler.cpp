#include	"../inc/ClientHandler.hpp"
#include	"../inc/EventHandler.hpp"
#include	<unistd.h>
#include	<sys/socket.h>

/* Constructor
** Create a handler for a newly accepted client connection.
**
** Stores the client's socket fd and initializes connection state.
** HTTP/1.1 defaults to persistent connections, so keep-alive is
** enabled until a request or response explicitly asks to close.
*/
ClientHandler::ClientHandler(int fd) : _fd(fd)
{
	//	add , _keepAlive(true) to function
	_keepAlive = true;
}

/* Destructor
** Cleanup for a client connection.
**
** Closes the socket associated with this client.
** Called when the connection is removed from the event loop.
*/
ClientHandler::~ClientHandler()
{
	close(_fd);
}

/*
** Read incoming data from the client socket.
**
** recv() copies available bytes from the kernel socket buffer into a
** temporary local buffer. The received data is then passed to the
** HttpRequest parser, which accumulates bytes across multiple reads
** until a complete HTTP request is available.
**
** Possible outcomes:
**
** - recv() == 0:
**     Client performed a clean disconnect.
**
** - recv() < 0:
**     Socket error occurred.
**
** - request parser detects an error:
**     An HTTP error response should be generated.
**
** - request becomes complete:
**     Build a response and store it in _outBuffer.
**     Actual sending happens later in handleWrite().
**
** This function only receives and parses data.
** It does not send anything back to the client.
*/
void	ClientHandler::handleRead()
{
	char	buffer[4096];
	ssize_t	bytesReceived = recv(_fd, buffer, sizeof(buffer), 0);;

	if (bytesReceived <= 0)
	{
		_setClose = true;
		return ;
	}
	_request.getData(std::string (buffer, bytesReceived));

	if (_request.hasError())
	{
		std::cout << "ERROR detected, would build error response here" << std::endl;
		//	needs implementation -> _outBuffer = errorResponse.serialize();
	}
	else if (_request.isComplete())
	{
		std::cout << "Request COMPLETE:"	<< std::endl;
		std::cout << "  method: "			<< _request.method << std::endl;
		std::cout << "  uri: "				<< _request.uri << std::endl;
		std::cout << "  body: "				<< _request.body << std::endl;

		//	temporary mesure
		_outBuffer = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";

		//	needs implementation ->	HttpResponse res = router.route(_request);
		//							_outBuffer = res.serialize();
		//	J'ai besoin de la partie de Jules pour creer le message de reponse 200 OK
	}
}

/*
** Send pending response data to the client.
**
** _outBuffer contains serialized HTTP response bytes waiting to be
** transmitted. Because sockets may be non-blocking, send() might
** write only part of the buffer.
**
** After each successful send:
**
** - remove the bytes that were transmitted
** - keep the remaining bytes for the next write event
**
** Once the entire response has been sent:
**
** - if keep-alive is enabled:
**       reset the request parser and wait for another request on the
**       same TCP connection
**
** - otherwise:
**       mark the connection for closure
**
** This function never generates responses; it only transmits bytes
** already prepared by handleRead().
*/
void	ClientHandler::handleWrite()
{
	// std::cout << "handleWrite is not implemented yet" << std::endl;

	if (_outBuffer.empty())
		return ; // nothing to send yet

	ssize_t	bytesSent = send(_fd, _outBuffer.c_str(), _outBuffer.size(), 0);

	if (bytesSent <= 0)
	{
		// error → cleanup
		_setClose = true;
		return ;
	}

	_outBuffer.erase(0, bytesSent);	//	delete what was sent

	if (_outBuffer.empty())
	{
		if (_keepAlive)
		{
			_request.reset();
			// reset _request for the next request on this connection
		}
		else
		{
			_setClose = true;
			// mark for closingnad
		}
	}
}

/*
** Return the socket file descriptor associated with this client.
**
** Used by the event loop (poll/select/epoll/kqueue) to identify
** which connection generated an event.
*/
int		ClientHandler::getFd() const
{
	// std::cout << "getFd is not implemented yet" << std::endl;
	return (_fd);
}

/*
** Check whether this connection should be closed.
**
** Returns true after a response has been fully sent and the
** connection is not supposed to remain alive.
**
** The event loop can use this flag to remove the handler and destroy
** the connection safely outside of the read/write callbacks.
*/
bool	ClientHandler::setClose() const
{
	return (_setClose);
}

/*
** Returns true when the output buffer has data waiting
** to be sent to the client.
** EventLoop uses this to register POLLOUT on this fd,
** so handleWrite() is only called when there is
** something to send.
*/
bool	ClientHandler::isWritable() const
{
	return (!_outBuffer.empty());
}
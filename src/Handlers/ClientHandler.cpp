#include	"../inc/ClientHandler.hpp"
#include	"../inc/EventHandler.hpp"
#include	"../inc/Config.hpp"
#include	"../inc/RequestHandler.hpp"
#include	"../inc/HttpResponse.hpp"
#include	<unistd.h>
#include	<sys/socket.h>

ClientHandler::ClientHandler(int fd, const ServerBlock &block) : _fd(fd), _config(block)
{
	_keepAlive = true;
	_isClosed = false;
	_request.setMaxBodySize(block.client_max_body_size);
}

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
	std::cout << "[CLIENTHANDLER] handleRead() fd=" << _fd << std::endl;

	char	buffer[4096];
	ssize_t	bytesReceived = recv(_fd, buffer, sizeof(buffer), 0);;

	if (bytesReceived <= 0)
	{
		_isClosed = true;
		return ;
	}

	//	temporary debug
	std::cout << "Received " << bytesReceived << " bytes:\n";
	for (ssize_t i = 0; i < bytesReceived; ++i)
	{
		unsigned char c = buffer[i];

		if (c == '\r')
			std::cout << "\\r";
		else if (c == '\n')
			std::cout << "\\n\n";
		else
			std::cout << c;
	}
	std::cout << "\n----- END OF CHUNK -----\n" << std::endl;

	_request.getData(std::string (buffer, bytesReceived));

	if (_request.hasError())
	{
		std::cout << "ERROR detected, would build error response here" << std::endl;
		//	TODO ??????? code error response a voir
		HttpResponse errorResponse = HttpResponse::make(500, "Internal Server Error");
		_outBuffer = errorResponse.serialize();
		_keepAlive = false;
	}
	else if (_request.isComplete())
	{
		//	check Connection header before setting response
		if (_request.headers.count("Connection") && _request.headers["Connection"] == "close")
			_keepAlive = false;

		std::cout << "Request COMPLETE:"	<< std::endl;
		std::cout << "  method: "			<< _request.method << std::endl;
		std::cout << "  uri: "				<< _request.uri << std::endl;
		std::cout << "  body: "				<< _request.body << std::endl;

		//	TODO (danny): temporary mesure
		//	needs implementation ->
		RequestHandler	createResponse(_request, _config);
		HttpResponse res = createResponse.processRequest();
		_outBuffer = res.serialize();
		//	I need Jule's response
		
		//_outBuffer = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
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
	std::cout << "[CLIENTHANDLER] handleWrite() fd=" << _fd << std::endl;

	if (_outBuffer.empty())
		return ;

	ssize_t	bytesSent = send(_fd, _outBuffer.c_str(), _outBuffer.size(), 0);

	if (bytesSent <= 0)
	{
		_isClosed = true;
		return ;
	}

	_outBuffer.erase(0, bytesSent);

	if (_outBuffer.empty())
	{
		if (_keepAlive)
			_request.reset();
		else
			_isClosed = true;
	}
}

int		ClientHandler::getFd() const
{
	return (_fd);
}

bool	ClientHandler::isClosed() const
{
	return (_isClosed);
}

bool	ClientHandler::isWritable() const
{
	return (!_outBuffer.empty());
}

#include	"../inc/ClientHandler.hpp"
#include	<unistd.h>
#include	<sys/socket.h>

ClientHandler::ClientHandler(int fd) : _fd(fd)
{
	//	add , _keepAlive(true) to function
	_keepAlive = true;
}

ClientHandler::~ClientHandler()
{
	close(_fd);
}

void	ClientHandler::handleRead()
{
	char	buffer[4096];
	ssize_t	bytesReceived = recv(_fd, buffer, sizeof(buffer), 0);;

	if (bytesReceived == 0)
	{
		//	clean disconnect
		close(_fd);
		return ;
	}
	if (bytesReceived < 0)
	{
		//	error
		close(_fd);
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
		_outBuffer = "temporary message: HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
		//	needs implementation ->	HttpResponse res = router.route(_request);
		//							_outBuffer = res.serialize();
		//	J'ai besoin de la partie de Jules pour creer le message de reponse 200 OK
	}
}

void	ClientHandler::handleWrite()
{
	// std::cout << "handleWrite is not implemented yet" << std::endl;

	if (_outBuffer.empty())
		return ; // nothing to send yet

	ssize_t	bytesSent = send(_fd, _outBuffer.c_str(), _outBuffer.size(), 0);

	if (bytesSent <= 0)
	{
		// error → cleanup
		return ;
	}

	_outBuffer.erase(0, bytesSent);
	//	delete what was sent

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

int		ClientHandler::getFd() const
{
	std::cout << "getFd is not implemented yet" << std::endl;
	return (0);
}

bool	ClientHandler::setClose() const
{
	return (_setClose);
}

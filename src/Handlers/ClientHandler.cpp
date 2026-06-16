#include	"../inc/ClientHandler.hpp"
#include	<unistd.h>
#include	<sys/socket.h>

ClientHandler::ClientHandler(int fd) : _fd(fd), _keepAlive(true) {}

ClientHandler::~ClientHandler()
{
	close(_fd);
}

void	ClientHandler::handleRead()
{
	char	buffer[4096];
	ssize_t	bytesReceived = recv(_fd, buffer, sizeof(buffer), 0);;

	if (bytesReceived <= 0)
	{
		// n == 0: client closed connection
		// n < 0: error
		// → mark this handler for removal/cleanup
		return ;
	}
	_request.getData(std::string (buffer, bytesReceived));

	if(_request.hasError())
	{
		// build error response into _outBuffer now
	}
	else if(_request.isComplete())
	{
		// pass _request to Router, get HttpResponse
		// serialize it into _outBuffer
	}
	// else: not enough data yet, wait for next handleRead() call
}
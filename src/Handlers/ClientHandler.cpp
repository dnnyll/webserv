#include	"../inc/ClientHandler.hpp"
#include	"../inc/EventHandler.hpp"
#include	"../inc/Config.hpp"
#include	"../inc/RequestHandler.hpp"
#include	"../inc/HttpResponse.hpp"
#include	"../inc/EventLoop.hpp"
#include	"../inc/CgiHandler.hpp"
#include	<unistd.h>
#include	<sys/socket.h>

ClientHandler::ClientHandler(int fd, const ServerBlock &block, EventLoop &reactor)
	: _fd(fd),
	_config(block),
	_reactor(reactor)
{
	_keepAlive = true;
	_isClosed = false;
	_request.setMaxBodySize(block.client_max_body_size);
}

ClientHandler::~ClientHandler()
{
	close(_fd);
}

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
		//	TODO (jules) code error response a voir
		// TODO (jules) si request a une erreur ne doit pas quitter
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


		// TODO(danny + jules): decide where status/flag CGI or RESPONSE is coming from!!

		RequestHandler	createResponse(_request, _config);
		HttpResponse res = createResponse.processRequest();
		if (res.statusMessage == "CGI")
		{
			std::cout << "[CLIENTHANDLER] CGI - create CgiHandler" << std::endl;
			// CgiHandler	*cgi = new CgiHandler(void);
			// _reactor.addHandler(cgi);
		}
		else
		{
			std::cout << "[CLIENTHANDLER] Response - serialize" << std::endl;
			_outBuffer = res.serialize();
		}
	}
}

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

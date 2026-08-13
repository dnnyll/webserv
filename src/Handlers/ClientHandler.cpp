#include	"../inc/ClientHandler.hpp"
#include	"../inc/EventHandler.hpp"
#include	"../inc/Config.hpp"
#include	"../inc/RequestHandler.hpp"
#include	"../inc/HttpResponse.hpp"
#include	"../inc/EventLoop.hpp"
#include	"../inc/Cgi/CgiContext.hpp"
#include	"../inc/Cgi/CgiLaunch.hpp"
#include	"../inc/Cgi/CgiWriteHandler.hpp"
#include	"../inc/Cgi/CgiReadHandler.hpp"
#include	<unistd.h>
#include	<sys/socket.h>

ClientHandler::ClientHandler(int fd, const ServerBlock &block, EventLoop &reactor)
	: _fd(fd),
	_config(block),
	_reactor(reactor),
	_clientAlive(NULL)	//	only allocated if/when a CGI request actually starts
{
	_keepAlive = true;
	_isClosed = false;
	_request.setMaxBodySize(block.client_max_body_size);
}

/*
	_clientAlive is refcounted: this handler holds one reference, and
	each in-flight CgiContext holds one. We flip alive=false so any
	surviving CgiReadHandler stops touching _outBuffer (about to be
	destroyed), then drop our reference. The flag object itself is
	only freed once the last CgiContext also releases it.
*/
ClientHandler::~ClientHandler()
{
	if (_clientAlive)
	{
		_clientAlive->alive = false;
		_clientAlive->release();
		_clientAlive = NULL;
	}
	close(_fd);
}

void	ClientHandler::handleRead()
{
	//std::cout << "[CLIENTHANDLER] handleRead() fd=" << _fd << std::endl;

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
		
		HttpResponse errorResponse;

		if (_request.getErrorReason() == BODY_TOO_LARGE)
			errorResponse = HttpResponse::make(413, "Payload Too Large");
		else if (_request.getErrorReason() == METHOD_NOT_ALLOWED)
		{
			errorResponse = HttpResponse::make(501, "Not Implemented");
		}
		else
			errorResponse = HttpResponse::make(400, "Bad Request");

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
		HttpResponse	res;
		CgiInfo 		cgi;
		
		ResponseType type = createResponse.processRequest(res, cgi);
		switch (type)
		{
			case CGI_PENDING:
			{
				std::cout << "[CLIENTHANDLER] CGI - create CgiHandler" << std::endl;
				// (void)cgi;
				if (!_clientAlive)
					_clientAlive = new CgiAlive();

				CgiContext	*ctx = new CgiContext();
				ctx->requestBody = _request.body;
				ctx->outBuffer   = &_outBuffer;
				ctx->clientAlive = _clientAlive;
				_clientAlive->addRef();	//	this CgiContext's reference

				if (!launchCgi(cgi, ctx))
				{
					//	setup failed before fork/pipes could be handed
					//	to any handler — nothing has taken ownership of
					//	ctx yet, so drop its reference on the flag and
					//	free it directly here
					ctx->clientAlive = NULL;
					_clientAlive->release();
					delete ctx;
					HttpResponse errorResponse = HttpResponse::make(500, "Internal Server Error");
					_outBuffer = errorResponse.serialize();
					break ;
				}

				_reactor.addHandler(new CgiWriteHandler(ctx));
				_reactor.addHandler(new CgiReadHandler(ctx));
				break ;
			}
			case RESPONSE_READY:
			{
				std::cout << "[CLIENTHANDLER] Response - serialize" << std::endl;
				_outBuffer = res.serialize();
				break ;
			}
		}
	}
}

void	ClientHandler::handleWrite()
{
	//std::cout << "[CLIENTHANDLER] handleWrite() fd=" << _fd << std::endl;

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

void	ClientHandler::markClosed()
{
	_isClosed = true;
}
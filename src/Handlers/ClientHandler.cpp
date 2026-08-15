#include	"../inc/ClientHandler.hpp"
#include	"../inc/EventHandler.hpp"
#include	"../inc/Config.hpp"
#include	"../inc/Router.hpp"
#include	"../inc/HttpResponse.hpp"
#include	"../inc/EventLoop.hpp"
#include	"../inc/Cgi/CgiContext.hpp"
#include	"../inc/Cgi/CgiLaunch.hpp"
#include	"../inc/Cgi/CgiWriteHandler.hpp"
#include	"../inc/Cgi/CgiReadHandler.hpp"
#include	<unistd.h>
#include	<sys/socket.h>
#include	<ctime>

static const time_t	IDLE_TIMEOUT = 30;

ClientHandler::ClientHandler(int fd, const ServerBlock &block, EventLoop &reactor)
	: _fd(fd),
	_config(block),
	_reactor(reactor),
	_clientAlive(NULL)	//	only allocated if/when a CGI request actually starts
{
	_keepAlive = true;
	_getClosed = false;
	_readClosed = false;
	_lastActivity = time(NULL);
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
		//	recv() == 0 is a FIN (half-close): the peer has stopped sending
		//	but may still be reading, so flush queued/pipelined responses
		//	before closing. Also defer closing while a CGI is still running
		//	(the CGI output lands in _outBuffer later). Only treat recv() < 0
		//	as a fatal error.
		if (bytesReceived == 0
			&& (!_outBuffer.empty()
				|| _request.hasPendingData()
				|| (_clientAlive && _clientAlive->refs > 1)))
		{
			if (_outBuffer.empty() && _request.hasPendingData())
			{
				_request.getData("");
				processRequest();
			}
			_readClosed = true;
			return ;
		}
		_getClosed = true;
		return ;
	}

	_lastActivity = time(NULL);

	// //	temporary debug
	// std::cout << "Received " << bytesReceived << " bytes:\n";
	// for (ssize_t i = 0; i < bytesReceived; ++i)
	// {
	// 	unsigned char c = buffer[i];

	// 	if (c == '\r')
	// 		std::cout << "\\r";
	// 	else if (c == '\n')
	// 		std::cout << "\\n\n";
	// 	else
	// 		std::cout << c;
	// }
	// std::cout << "\n----- END OF CHUNK -----\n" << std::endl;

	_request.getData(std::string (buffer, bytesReceived));

	processRequest();
}

void	ClientHandler::processRequest()
{
	if (_request.hasError())
	{
		
		HttpResponse errorResponse;

		if (_request.getErrorReason() == BODY_TOO_LARGE)
			errorResponse = Router::makeError(413, "Payload Too Large", _config);
		else if (_request.getErrorReason() == HEADER_TOO_LARGE)
			errorResponse = Router::makeError(431, "Request Header Fields Too Large", _config);
		else if (_request.getErrorReason() == URI_TOO_LONG)
			errorResponse = Router::makeError(414, "URI Too Long", _config);
		else if (_request.getErrorReason() == HTTP_VERSION_NOT_SUPPORTED)
			errorResponse = Router::makeError(505, "HTTP Version Not Supported", _config);
		else if (_request.getErrorReason() == METHOD_NOT_ALLOWED)
			errorResponse = Router::makeError(501, "Not Implemented", _config);
		else
			errorResponse = Router::makeError(400, "Bad Request", _config);

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

		Router			createResponse(_request, _config);
		HttpResponse	res;
		CgiInfo 		cgi;
		
		ResponseType type = createResponse.processRequest(res, cgi);
		
		switch (type)
		{
			case CGI_PENDING:
			{
				std::cout << "[HANDLEREAD] CGI - create CgiHandler" << std::endl;

				if (!_clientAlive)
					_clientAlive = new CgiAlive();

				CgiContext	*ctx = new CgiContext();
				ctx->requestBody = _request.body;
				ctx->outBuffer   = &_outBuffer;
				ctx->clientAlive = _clientAlive;
				ctx->config      = &_config;
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
					HttpResponse errorResponse = Router::makeError(500, "Internal Server Error", _config);
					_outBuffer = errorResponse.serialize();
					break ;
				}

				_reactor.addHandler(new CgiWriteHandler(ctx));
				_reactor.addHandler(new CgiReadHandler(ctx));
				break ;
			}
			case RESPONSE_READY:
			{
				std::cout << "[HANDLEREAD] Response - serialize" << std::endl;
				_outBuffer = res.serialize();
				break ;
			}
		}
	}
}

void	ClientHandler::handleWrite()
{
	if (_outBuffer.empty())
		return ;

	ssize_t	bytesSent = send(_fd, _outBuffer.c_str(), _outBuffer.size(), 0);

	if (bytesSent <= 0)
	{
		_getClosed = true;
		return ;
	}

	_lastActivity = time(NULL);

	_outBuffer.erase(0, bytesSent);

	if (_outBuffer.empty())
	{
		if (_keepAlive)
		{
			_request.reset();

			//	a pipelined request may already be sitting in the parser's
			//	buffer — feed it back instead of waiting for the next recv()
			if (_request.hasPendingData())
			{
				_request.getData("");
				processRequest();
			}
			else if (_readClosed)
				_getClosed = true;
		}
		else
			_getClosed = true;
	}
}

int		ClientHandler::getFd() const
{
	return (_fd);
}

bool	ClientHandler::getClosed() const
{
	return (_getClosed);
}

bool	ClientHandler::isWritable() const
{
	return (!_outBuffer.empty());
}

void	ClientHandler::setClosed()
{
	_getClosed = true;
}

bool	ClientHandler::shouldTimeout() const
{
	return (!_request.isComplete()
			&& _outBuffer.empty()
			&& time(NULL) - _lastActivity >= IDLE_TIMEOUT);
}

void	ClientHandler::onTimeout()
{
	HttpResponse	errorResponse = Router::makeError(408, "Request Timeout", _config);

	_outBuffer = errorResponse.serialize();
	_keepAlive = false;
}

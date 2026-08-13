#include	"../inc/Cgi/CgiWriteHandler.hpp"
#include	<unistd.h>
#include	<iostream>

CgiWriteHandler::CgiWriteHandler(CgiContext *ctx) : _ctx(ctx)
{
	_ctx->addRef();
}

CgiWriteHandler::~CgiWriteHandler()
{
	_ctx->release();
}

int	CgiWriteHandler::getFd() const
{
	return (_ctx->pipeInWrite);
}

bool	CgiWriteHandler::isWritable() const
{
	return (!_ctx->writeDone);
}

/*
	Reports closed once writing is finished, OR once this invocation
	has been running longer than TIMEOUT_SECONDS. On timeout, force
	the pipe closed here — CgiReadHandler will notice the child is
	gone (EOF / error) on its own next handleRead() and reap it.
*/
bool	CgiWriteHandler::getClosed() const
{
	if (_ctx->writeDone)
		return (true);
	if (hasTimedOut())
	{
		std::cout << "[CGIWRITE] timed out, closing pipeInWrite" << std::endl;
		if (_ctx->pipeInWrite != -1)
		{
			close(_ctx->pipeInWrite);
			_ctx->pipeInWrite = -1;
		}
		_ctx->writeDone = true;
		return (true);
	}
	return (false);
}

bool	CgiWriteHandler::hasTimedOut() const
{
	return (time(NULL) - _ctx->startTime > TIMEOUT_SECONDS);
}

void	CgiWriteHandler::handleWrite()
{
	if (_ctx->writeDone)
		return ;

	size_t	remaining = _ctx->requestBody.size() - _ctx->bodySent;
	ssize_t	n = write(_ctx->pipeInWrite,
				_ctx->requestBody.c_str() + _ctx->bodySent, remaining);

	if (n < 0)
	{
		//	No errno inspection here: the subject forbids checking errno
		//	after a read/write, and with handleWrite() gated on POLLOUT
		//	the pipe is writable when we get here. A genuine failure
		//	(e.g. the child exiting) surfaces as POLLERR and is handled
		//	by setClosed(); anything transient is retried on the next
		//	POLLOUT event.
		return ;
	}

	_ctx->bodySent += n;
	if (_ctx->bodySent == _ctx->requestBody.size())
	{
		//	closing sends EOF on the child's stdin
		close(_ctx->pipeInWrite);
		_ctx->pipeInWrite = -1;
		_ctx->writeDone = true;
	}
}

void	CgiWriteHandler::handleRead()
{
	//	this handler is only ever polled for POLLOUT; nothing to do here
}

void	CgiWriteHandler::setClosed()
{
	if (_ctx->writeDone)
		return ;

	std::cout << "[CGIWRITE] POLLERR, closing pipeInWrite" << std::endl;
	if (_ctx->pipeInWrite != -1)
	{
		close(_ctx->pipeInWrite);
		_ctx->pipeInWrite = -1;
	}
	_ctx->writeDone = true;
}

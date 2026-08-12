#include	"../inc/Cgi/CgiReadHandler.hpp"
#include	<unistd.h>
#include	<sys/wait.h>
#include	<cerrno>
#include	<iostream>

CgiReadHandler::CgiReadHandler(CgiContext *ctx) : _ctx(ctx)
{
	_ctx->addRef();
}

CgiReadHandler::~CgiReadHandler()
{
	_ctx->release();
}

int	CgiReadHandler::getFd() const
{
	return (_ctx->pipeOutRead);
}

bool	CgiReadHandler::isWritable() const
{
	return (false);
}

bool	CgiReadHandler::isClosed() const
{
	if (_ctx->readDone)
		return (true);
	if (hasTimedOut())
	{
		std::cout << "[CGIREAD] timed out, killing CGI child" << std::endl;
		if (_ctx->pipeOutRead != -1)
		{
			close(_ctx->pipeOutRead);
			_ctx->pipeOutRead = -1;
		}
		if (_ctx->pid > 0)
			kill(_ctx->pid, SIGKILL);
		reapChild();
		_ctx->readDone = true;
		return (true);
	}
	return (false);
}

bool	CgiReadHandler::hasTimedOut() const
{
	return (time(NULL) - _ctx->startTime > TIMEOUT_SECONDS);
}

/*
	Reap with WNOHANG so this never blocks the reactor thread, even
	though by the time we call this (EOF on stdout, or a forced kill
	on timeout) the child is expected to be finished or finishing
	very shortly.
*/
void	CgiReadHandler::reapChild() const
{
	if (_ctx->pid <= 0)
		return ;

	int	status;
	waitpid(_ctx->pid, &status, WNOHANG);
	_ctx->pid = -1;
}

void	CgiReadHandler::handleRead()
{
	char	buf[4096];
	ssize_t	n = read(_ctx->pipeOutRead, buf, sizeof(buf));

	if (n > 0)
	{
		if (_ctx->clientAlive && *_ctx->clientAlive)
			_ctx->outBuffer->append(buf, n);
		//	if the client is gone, drop the bytes on the floor —
		//	we still need to keep draining the pipe so the child
		//	doesn't block on a full stdout buffer before exiting
		return ;
	}

	if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		return ;

	//	n == 0 (EOF) or a real read error: this side is done either way
	std::cout << "[CGIREAD] CGI output finished (fd=" << _ctx->pipeOutRead << ")" << std::endl;
	close(_ctx->pipeOutRead);
	_ctx->pipeOutRead = -1;
	reapChild();
	_ctx->readDone = true;
}

void	CgiReadHandler::handleWrite()
{
	//	this handler is only ever polled for POLLIN; nothing to do here
}

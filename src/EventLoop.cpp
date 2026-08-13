#include	"../inc/EventLoop.hpp"
#include	<cstddef>
#include	<iostream>
#include	<csignal>
#include	<cerrno>
#include	<cstring>

static bool	g_isRunning = true;

static void	signalHandler(int sig)
{
	(void)sig;
	g_isRunning = false;
}

EventLoop::EventLoop(){}

EventLoop::~EventLoop()
{
	size_t	i = 0;

	while(i < _handlers.size())
	{
		delete _handlers[i];
		i++;
	}
	_handlers.clear();
}

void	EventLoop::addHandler(EventHandler *handler)
{
	_handlers.push_back(handler);
}

void EventLoop::run()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN);

	while (g_isRunning)
	{
		buildPollFds();

		if (_pollfds.empty())
			continue ;

		int pollReady = poll(&_pollfds[0], _pollfds.size(), 1000);

		if (pollReady == -1)
		{
			if (!g_isRunning)
				break ;

			if (errno == EINTR)
				continue ;

			break ;
		}
		dispatch();
		removeClosedHandlers();
	}
}

void	EventLoop::buildPollFds()
{
	size_t	i = 0;

	_pollfds.clear();
	while (i < _handlers.size())
	{
		struct	pollfd pfd;
		pfd.fd  = _handlers[i]->getFd();
		pfd.events = POLLIN;
		pfd.revents = 0;

		if (_handlers[i]->isWritable())
			pfd.events |= POLLOUT;

		_pollfds.push_back(pfd);
		i++;
	}
}

void	EventLoop::dispatch()
{
	size_t	i = 0;

	while (i < _pollfds.size())
	{
		short	rev = _pollfds[i].revents;

		//	POLLHUP is the EOF condition: the write end of the pipe was
		//	closed (CGI child exited) or the peer hung up. Treat it as
		//	readable so handleRead() processes the EOF / reap instead of
		//	leaving the handler to spin until its timeout.
		if ((rev & (POLLIN | POLLHUP)) && _pollfds[i].fd >= 0)
			_handlers[i]->handleRead();

		//	only write when poll() actually reported the fd writable.
		//	(An isWritable() short-circuit here could run a write before
		//	the fd is ready, which is forbidden by the subject and would
		//	force an errno/EAGAIN check we are not allowed to make.)
		if (rev & POLLOUT)
			_handlers[i]->handleWrite();

		if (rev & (POLLERR | POLLNVAL))
			_handlers[i]->setClosed();

		i++;
	}
}

void	EventLoop::removeClosedHandlers()
{
	size_t	i = 0;

	while (i < _handlers.size())
	{
		if (_handlers[i]->getClosed())
		{
			delete _handlers[i];
			_handlers.erase(_handlers.begin() + i);
		}
		else
			i++;
	}
}

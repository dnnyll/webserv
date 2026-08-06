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
	// << "[EVENTLOOP] handler added, total: " << _handlers.size() << std::endl;
}

/*
	Main event-processing loop.
	Runs indefinitely while the server is marked as running:
	1) rebuild the pollfd array from the current set of active handlers
	2) block in poll() until at least one fd becomes ready (timeout = -1)
	3) dispatch ready events to their corresponding handlers
	4) remove handlers that have been marked for closing
	If poll() fails:
	- when errno == EINTR, the loop continues (signal interrupted the wait)
	- for other errors, the loop breaks and the server shuts down
*/
void EventLoop::run()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN); // avoid dying on write() to a pipe whose reader is gone

	while (g_isRunning)
	{
		buildPollFds();

		// << "[EVENTLOOP] polling " << _handlers.size() << " handlers | pollfds.size()=" << _pollfds.size() << std::endl;

		if (_pollfds.empty())
		{
			// << "[EVENTLOOP] _pollfds.empty(), continue" << std::endl;
			continue ;
		}

		int pollReady = poll(&_pollfds[0], _pollfds.size(), 1000);

		if (pollReady == -1)
		{
			// << "[EVENTLOOP][RUN] poll() error: errno=" << errno << " (" << ::strerror(errno) << ")" << std::endl;

			if (!g_isRunning)
				break ;

			// EINTR = interrupted by signal while waiting (not necessarily fatal)
			if (errno == EINTR)
				continue ;

			break ;
		}

		if (pollReady == 0)
		{
			// << "[EVENTLOOP][RUN] poll() timeout (1000ms) - no fds ready"
					//<< std::endl;
		}
		else
		{
			// << "[EVENTLOOP][RUN] " << pollReady << " fd(s) ready"
					//<< std::endl;
		}

		// Even on timeout, dispatch()/removeClosedHandlers() run so timeouts in CgiWriteHandler/CgiReadHandler can still be evaluated.
		dispatch();
		removeClosedHandlers();

		// << "[EVENTLOOP] " << _handlers.size() << " handlers remaining" << std::endl;
	}

	// << "Server shutting down cleanly..." << std::endl;
}


/*
	Build the pollfd array from the current list of handlers.
	Called at the start of each event loop iteration to
	reflect the current state of all active handlers.
	Clears the previous array and rebuilds it fresh since
	handlers can be added or removed between iterations.

	POLLIN  is always registered — every handler can receive data.
	POLLOUT is only registered when the handler has data to send,
	to avoid waking up poll() unnecessarily.
*/
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

/*
	Dispatch events to the appropriate handlers after poll() returns.

	Iterates through the pollfd array and checks revents on each entry:
	POLLIN  — data is available to read  → handleRead()
	POLLOUT — fd is ready to write       → handleWrite()
	POLLERR — error on fd                → mark handler for closing

	_handlers[i] and _pollfds[i] always correspond to the same handler
	since both are built in the same order by buildPollFds().
*/
void	EventLoop::dispatch()
{
	size_t	i = 0;

	while (i < _pollfds.size())
	{
		// << "[DISPATCH] fd=" << _pollfds[i].fd << " revents=" << _pollfds[i].revents << std::endl;
		if (_pollfds[i].revents & POLLIN && _pollfds[i].fd >= 0)
			_handlers[i]->handleRead();

		//	check immediately if there's something to write and call handleWrite() in the same dispatch cycle.
		if (_pollfds[i].revents & POLLOUT || _handlers[i]->isWritable())
			_handlers[i]->handleWrite();

		if (_pollfds[i].revents & POLLERR)
			_handlers[i]->isClosed();

		i++;
	}
}

/*
	Remove and destroy handlers that are marked for closing.
	Walk through the handler list and, when a handler reports it is closed, delete it and erase it from the vector immediately.
	Since erase() shifts remaining elements left, only increment the index when no removal happens to avoid skipping handlers.
*/
void	EventLoop::removeClosedHandlers()
{
	size_t	i = 0;

	while (i < _handlers.size())
	{
		if (_handlers[i]->isClosed())
		{
			// << "[EVENTLOOP] removing handler fd=" << _handlers[i]->getFd() << std::endl;
			delete _handlers[i];
			_handlers.erase(_handlers.begin() + i);
		}
		else
			i++;
	}
}

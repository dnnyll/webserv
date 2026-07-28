#include	"../inc/EventLoop.hpp"
#include	<cstddef>
#include	<iostream>
#include	<csignal>

static bool	g_isRunning = true;

static void	signalHandler(int sig)
{
	(void)sig;
	std::cout << std::endl;
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
	std::cout << "[EVENTLOOP] handler added, total: " << _handlers.size() << std::endl;
}

/*
** Main event-processing loop.
**
** Repeats indefinitely:
**     1. rebuild the pollfd array from active handlers
**     2. wait in poll() until at least one fd is ready
**     3. dispatch ready events to their handlers
**     4. remove handlers marked for closing
**
** poll() blocks indefinitely (-1 timeout) until an event occurs.
** If poll() fails, the iteration is skipped and the loop continues.
*/
void	EventLoop::run()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	while (g_isRunning)
	{
		buildPollFds();
		std::cout << "[EVENTLOOP] polling " << _handlers.size() << " handlers" << std::endl;
		int	pollReady = poll(&_pollfds[0], _pollfds.size(), -1);
	
		if(pollReady == -1)
		{
			if (!g_isRunning)
			{
				std::cout << "[EVENTLOOP] signal killed (ctrl+c)" << std::endl;
				break ;
			}
			std::cout << "[EVENTLOOP] poll() error" << std::endl;
			//	TODO (danny) : HANDLE ERROR for edge case poll()
			std::cout << "edgecase poll() returned -1. handle error" << std::endl;
			continue ;
		}
		std::cout << "[EVENTLOOP] " << pollReady << " fd(s) ready" << std::endl;
		dispatch();
		removeClosedHandlers();
		std::cout << "[EVENTLOOP] " << _handlers.size() << " handlers remaining" << std::endl;
		// break ;
	}
	std::cout << "Server shutting down cleanly..." << std::endl;
}

/*
** Build the pollfd array from the current list of handlers.
**
** Called at the start of each event loop iteration to
** reflect the current state of all active handlers.
** Clears the previous array and rebuilds it fresh since
** handlers can be added or removed between iterations.
**
** POLLIN  is always registered — every handler can receive data.
** POLLOUT is only registered when the handler has data to send,
** to avoid waking up poll() unnecessarily.
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
** Dispatch events to the appropriate handlers after poll() returns.
**
** Iterates through the pollfd array and checks revents on each entry:
** POLLIN  — data is available to read  → handleRead()
** POLLOUT — fd is ready to write       → handleWrite()
** POLLERR — error on fd                → mark handler for closing
**
** _handlers[i] and _pollfds[i] always correspond to the same handler
** since both are built in the same order by buildPollFds().
*/
void	EventLoop::dispatch()
{
	size_t	i = 0;

	while (i < _pollfds.size())
	{
		std::cout << "[DISPATCH] fd=" << _pollfds[i].fd << " revents=" << _pollfds[i].revents << std::endl;
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
** Remove and destroy handlers that have been marked for closing.
**
** Iterates through the handler list and checks each handler's
** close state. Closed handlers are deleted and removed from
** the vector immediately.
**
** erase() shifts all following elements one position to the left,
** so the index is only incremented when no removal occurs.
** This ensures no handlers are skipped during iteration.
*/
void	EventLoop::removeClosedHandlers()
{
	size_t	i = 0;

	while (i < _handlers.size())
	{
		if (_handlers[i]->isClosed())
		{
			std::cout << "[EVENTLOOP] removing handler fd=" << _handlers[i]->getFd() << std::endl;
			delete _handlers[i];
			_handlers.erase(_handlers.begin() + i);
		}
		else
			i++;
	}
}

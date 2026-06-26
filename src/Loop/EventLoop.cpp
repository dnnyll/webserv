#include	"../inc/EventLoop.hpp"
#include	<cstddef>
#include	<iostream>

/*
** Construct an empty EventLoop.
**
** Handlers are added later through addHandler().
*/
EventLoop::EventLoop(){}

/*
** Destroy the EventLoop and release all remaining handlers.
**
** Any handlers still owned by the event loop are deleted
** before the handler list is cleared.
**
** The EventLoop assumes ownership of every handler passed
** to addHandler(), making it responsible for cleanup.
*/

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

/*
** Register a new handler with the event loop.
**
** The handler will be included in future poll() calls
** and receive events when its file descriptor becomes ready.
**
** Ownership is transferred to the EventLoop, which is
** responsible for deleting the handler during cleanup.
*/
void	EventLoop::addHandler(EventHandler *handler)
{
	_handlers.push_back(handler);
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
	while (true)
	{
		buildPollFds();
		int	pollReady = poll(&_pollfds[0], _pollfds.size(), -1);
	
		if(pollReady == -1)
		{
			std::cout << "edgecase poll() returned -1. handle error" << std::endl;
			continue ;	//	skips dispatch
		}
		dispatch();
		removeClosedHandlers();
	}
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
** Dispatch events to the appropriate handlers after p
oll() returns.
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
		if (_pollfds[i].revents & POLLIN)
			_handlers[i]->handleRead();

		if (_pollfds[i].revents & POLLOUT || _handlers[i]->isWritable())
			_handlers[i]->handleWrite();
		//	check immediately if there's something to write and call handleWrite() in the same dispatch cycle.

		if (_pollfds[i].revents & POLLERR)
			_handlers[i]->isClosed(); // mark for cleanup

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
			delete _handlers[i];
			_handlers.erase(_handlers.begin() + i);	//	ereases element from vector and shifts left
		}
		else
			i++;
	}
}

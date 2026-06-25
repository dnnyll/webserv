#include	"../inc/EventLoop.hpp"
#include	<cstddef>

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
	size_t i = 0;

	while (i < _pollfds.size())
	{
		if (_pollfds[i].revents & POLLIN)
			_handlers[i]->handleRead();

		if (_pollfds[i].revents & POLLOUT)
			_handlers[i]->handleWrite();

		if (_pollfds[i].revents & POLLERR)
			_handlers[i]->setClose(); // mark for cleanup

		i++;
	}
}
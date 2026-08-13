#ifndef		EVENTLOOP_HPP
# define	EVENTLOOP_HPP

#include	"../inc/EventHandler.hpp"
#include	<vector>
#include	<poll.h>

class	EventLoop
{
	public:
		EventLoop();
		~EventLoop();

		void	addHandler(EventHandler *handler);
		void	run();

	private:
		std::vector<EventHandler*>	_handlers;
		std::vector<struct pollfd>	_pollfds;

		void			buildPollFds();
		void			dispatch();
		void			removeClosedHandlers();
};

#endif

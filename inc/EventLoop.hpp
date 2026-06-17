#ifndef		EVENTLOOP_HPP
# define	EVENTLOOP_HPP

#include	"../inc/EventHandler.hpp"

class	EventLoop
{
	public:
		EventLoop();
		~EventLoop();

		void	addHandler(EventHandler *handler);
		void	run();

	private:
		std::vector<IEventHandler*> _handlers;
		std::vector<pollfd>         _pollfds;

		void	buildPollFds();
		void	dispatch();
		void	removeClosedHandlers();
};

#endif

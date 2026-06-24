#ifndef		EVENTLOOP_HPP
# define	EVENTLOOP_HPP

#include	"../inc/EventHandler.hpp"
#include	<vector>
#include	<poll.h>

// So what is EventLoop actually doing, mechanically?
// loop forever:
//     1. ask poll(): "of all these fds, which ones are ready?"
//     2. for each fd that's ready:
//          find which handler owns it
//          tell that handler to act
// EventLoop doesn't know or care what happens when it tells a handler to act - 
// that's the whole point of polymorphism. 
// It just says "you're ready, go" and trusts the handler to do the right thing.

//check docs: webserv_Handlers_

class	EventLoop
{
	public:
		EventLoop();
		~EventLoop();

		void addHandler(EventHandler *handler);
		void run();

	private:
		std::vector<EventHandler*>  _handlers;
		std::vector<struct pollfd>  _pollfds;

		void buildPollFds();
		void dispatch();
		void removeClosedHandlers();
};

#endif

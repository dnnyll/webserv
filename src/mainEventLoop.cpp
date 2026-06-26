#include	"../inc/EventLoop.hpp"
#include	"../inc/AcceptHandler.hpp"

int	main()
{
	EventLoop		reactor;
	AcceptHandler	listener(8080, reactor);
	reactor.addHandler(&listener);
	reactor.run();
	return (0);
}

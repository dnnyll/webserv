#ifndef		ACCEPTHANDLER_HPP
# define	ACCEPTHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	"../inc/EventLoop.hpp"

class	AcceptHandler : public	EventHandler
{
	public:
		AcceptHandler(int port, EventLoop &reactor);
		//means "a reference to an EventLoop, and I'll refer to it as reactor inside this class." 
		//The & binds to the type, not the variable name.
		~AcceptHandler();
		//	overwritten methods
		void	handleRead();
		void	handleWrite();
		int		getFd() const;
		bool	setClose() const;

	private:
		int		_fd;
};

#endif
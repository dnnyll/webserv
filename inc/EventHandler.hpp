#ifndef		EVENTHANDLER_HPP
#define		EVENTHANDLER_HPP


//	the event loop just wants to know if the fd is ready so it can
//	call it's handler

class	EventHandler
{
	public:
		virtual			~EventHandler() {}
		virtual void	handleRead()  = 0;
		virtual void	handleWrite() = 0;
		virtual int		getFd() const = 0;
		virtual bool	setClose() const = 0;
};

#endif
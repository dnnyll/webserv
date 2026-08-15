#ifndef		EVENTHANDLER_HPP
#define		EVENTHANDLER_HPP

class	EventHandler
{
	public:
		virtual			~EventHandler() {}
		virtual void	handleRead()  = 0;
		virtual void	handleWrite() = 0;
		virtual int		getFd() const = 0;
		virtual bool	getClosed() const = 0;
		virtual void	setClosed() = 0;
		virtual bool	isWritable() const = 0;
		virtual bool	shouldTimeout() const { return (false); }
		virtual void	onTimeout() { }
};

#endif

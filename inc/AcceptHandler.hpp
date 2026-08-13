#ifndef		ACCEPTHANDLER_HPP
# define	ACCEPTHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	"../inc/EventLoop.hpp"
#include	"../inc/Config.hpp"
#include	<string>

class	AcceptHandler : public	EventHandler
{
	private:
		int			_fd;
		EventLoop	&_reactor;
		ServerBlock _serverBlock;

	public:
		AcceptHandler(const ServerBlock &block, EventLoop &reactor);
		~AcceptHandler();

		void	handleRead();
		void	handleWrite();
		int		getFd() const;
		bool	getClosed() const;
		void	setClosed();
		bool	isWritable() const;

	private:
		void	setupSocket(int port, const std::string &host);
};

#endif

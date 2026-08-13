#ifndef		CGIWRITEHANDLER_HPP
# define	CGIWRITEHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	"../inc/Cgi/CgiContext.hpp"

class	CgiWriteHandler : public	EventHandler
{
	public:
		CgiWriteHandler(CgiContext *ctx);
		~CgiWriteHandler();

		//	virtual overwritten methods
		void	handleRead();		//	no-op: this handler is never watched for POLLIN
		void	handleWrite();
		int		getFd() const;
		bool	getClosed() const;
		bool	isWritable() const;
		void	setClosed();

	private:
		CgiContext	*_ctx;

		static const time_t	TIMEOUT_SECONDS = 30;

		bool	hasTimedOut() const;
};

#endif

#ifndef		CGIREADHANDLER_HPP
# define	CGIREADHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	"../inc/Cgi/CgiContext.hpp"

/*
** CgiReadHandler
**
** Wraps the read end of the pipe receiving the CGI child's stdout.
**
** Responsibilities:
** - read output from the child and append it to the ClientHandler's
**   _outBuffer (via CgiContext::outBuffer), guarded by clientAlive
** - on EOF, close the pipe, reap the child (waitpid), and mark itself
**   closed
** - report getClosed() once reading is finished OR the invocation has
**   timed out
**
** Does NOT own the CgiContext — see CgiContext.hpp for the refcount
** ownership model shared with CgiWriteHandler.
*/
class	CgiReadHandler : public	EventHandler
{
	public:
		CgiReadHandler(CgiContext *ctx);
		~CgiReadHandler();

		//	virtual overwritten methods
		void	handleRead();
		void	handleWrite();		//	no-op: this handler is never watched for POLLOUT
		int		getFd() const;
		bool	getClosed() const;
		bool	isWritable() const;
		void	setClosed();


	private:
		CgiContext	*_ctx;

		static const time_t	TIMEOUT_SECONDS = 30;

		bool	hasTimedOut() const;
		void	reapChild(bool block) const;
};

#endif

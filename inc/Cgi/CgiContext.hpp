#ifndef		CGICONTEXT_HPP
# define	CGICONTEXT_HPP

#include	<string>
#include	<sys/types.h>
#include	<ctime>

struct	CgiContext
{
	pid_t			pid;

	int				pipeInWrite;	//	parent -> CGI stdin, -1 once closed
	int				pipeOutRead;	//	CGI stdout -> parent, -1 once closed

	std::string		requestBody;
	size_t			bodySent;

	std::string		*outBuffer;		//	non-owning, points into ClientHandler::_outBuffer
	bool			*clientAlive;	//	non-owning, shared flag: false once ClientHandler is destroyed

	bool			writeDone;
	bool			readDone;

	time_t			startTime;		//	used for timeout detection in isClosed()

	int				refCount;

	CgiContext();

	void	addRef();
	void	release();
};

#endif

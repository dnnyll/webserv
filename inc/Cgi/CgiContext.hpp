#ifndef		CGICONTEXT_HPP
# define	CGICONTEXT_HPP

#include	<string>
#include	<sys/types.h>
#include	<ctime>
#include	"../Config.hpp"

/*
	Shared "is the client still alive" flag.

	Owned by no single side: the ClientHandler holds one reference and
	each in-flight CgiContext holds one. The flag is only freed once
	both are done with it, so neither side can touch freed memory.
*/
struct	CgiAlive
{
	bool	alive;
	int		refs;

	CgiAlive() : alive(true), refs(1) {}

	void	addRef() { refs++; }
	void	release()
	{
		refs--;
		if (refs == 0)
			delete this;
	}
};

struct	CgiContext
{
	pid_t			pid;

	int				pipeInWrite;	//	parent -> CGI stdin, -1 once closed
	int				pipeOutRead;	//	CGI stdout -> parent, -1 once closed

	std::string		requestBody;
	size_t			bodySent;

	std::string		output;			//	raw CGI stdout, accumulated while the child runs
	std::string		*outBuffer;		//	non-owning, points into ClientHandler::_outBuffer
	CgiAlive		*clientAlive;	//	shared flag: alive=false once ClientHandler is destroyed
	const ServerBlock	*config;	//	server block, for custom error pages

	bool			writeDone;
	bool			readDone;

	time_t			startTime;		//	used for timeout detection in getClosed()

	int				exitStatus;		//	raw waitpid() status once the child is reaped,
									//	-1 while it is still running / unknown

	int				refCount;

	CgiContext();

	void	addRef();
	void	release();
};

#endif

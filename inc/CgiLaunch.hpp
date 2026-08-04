#ifndef		CGILAUNCH_HPP
# define	CGILAUNCH_HPP

#include	"../inc/CgiContext.hpp"
#include	<string>
#include	<vector>

/*
	CgiInfo is produced by RequestHandler::getCgiInfo() and consumed
	here to actually fork/exec the CGI process.
*/
struct	CgiInfo
{
	std::string					scriptPath;
	std::string					workingDirectory;
	std::string					interpreterPath;
	std::vector<std::string>	env;
};

/*
** launchCgi
**
** Creates the two pipes, forks, execve()s the CGI script in the
** child, and fills in ctx->pid / ctx->pipeInWrite / ctx->pipeOutRead
** in the parent.
**
** If ctx->requestBody is empty, closes pipeInWrite immediately and
** sets ctx->writeDone = true, so CgiWriteHandler doesn't stall
** forever waiting to send nothing.
**
** Returns false if pipe()/fork() setup failed (caller should treat
** this as CGI failure and fall back to an error response).
*/
bool	launchCgi(const CgiInfo &info, CgiContext *ctx);

#endif
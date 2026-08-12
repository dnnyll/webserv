#include	"../inc/CgiLaunch.hpp"
#include	<unistd.h>
#include	<fcntl.h>
#include	<sys/wait.h>
#include	<cstdlib>
#include	<iostream>
#include	<cerrno>
#include	<cstring>

static char	**buildEnvp(const std::vector<std::string> &env)
{
	char	**envp = new char*[env.size() + 1];
	size_t	i = 0;

	while (i < env.size())
	{
		envp[i] = new char[env[i].size() + 1];
		std::strcpy(envp[i], env[i].c_str());
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

static void	freeEnvp(char **envp)
{
	size_t	i = 0;

	while (envp[i])
	{
		delete[] envp[i];
		i++;
	}
	delete[] envp;
}

/*
	launchCgi

	Sets up the two pipes, forks, and execve()s the CGI script in the
	child. On success, ctx->pid / ctx->pipeInWrite / ctx->pipeOutRead
	are filled in for the parent side.

	If the request has no body (GET, or empty POST), pipeInWrite is
	closed immediately and ctx->writeDone is set to true right away,
	so CgiWriteHandler never gets registered stuck waiting to send
	nothing (which would leave it unwritable forever and never
	removed by the EventLoop).
*/
bool	launchCgi(const CgiInfo &info, CgiContext *ctx)
{
	int	pipeIn[2];
	int	pipeOut[2];

	if (pipe(pipeIn) < 0)
	{
		std::cerr << "[CGI] pipe(pipeIn) failed: " << strerror(errno) << std::endl;
		return (false);
	}
	if (pipe(pipeOut) < 0)
	{
		std::cerr << "[CGI] pipe(pipeOut) failed: " << strerror(errno) << std::endl;
		close(pipeIn[0]);
		close(pipeIn[1]);
		return (false);
	}

	pid_t	pid = fork();

	if (pid < 0)
	{
		std::cerr << "[CGI] fork() failed: " << strerror(errno) << std::endl;
		close(pipeIn[0]);
		close(pipeIn[1]);
		close(pipeOut[0]);
		close(pipeOut[1]);
		return (false);
	}

	if (pid == 0)
	{
		//	---- CHILD ----
		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);

		close(pipeIn[0]);
		close(pipeIn[1]);
		close(pipeOut[0]);
		close(pipeOut[1]);

		if (!info.workingDirectory.empty())
			chdir(info.workingDirectory.c_str());

		char	*argv[3];
		argv[0] = const_cast<char*>(info.interpreterPath.c_str());
		argv[1] = const_cast<char*>(info.execScriptName.c_str());
		argv[2] = NULL;

		char	**envp = buildEnvp(info.env);

		execve(info.interpreterPath.c_str(), argv, envp);

		//	execve only returns on failure
		std::cerr << "[CGI] execve() failed: " << strerror(errno) << std::endl;
		freeEnvp(envp);
		std::exit(1);
	}

	//	---- PARENT ----
	close(pipeIn[0]);
	close(pipeOut[1]);

	ctx->pid         = pid;
	ctx->pipeInWrite = pipeIn[1];
	ctx->pipeOutRead = pipeOut[0];

	fcntl(ctx->pipeInWrite, F_SETFL, O_NONBLOCK);
	fcntl(ctx->pipeOutRead, F_SETFL, O_NONBLOCK);

	if (ctx->requestBody.empty())
	{
		close(ctx->pipeInWrite);
		ctx->pipeInWrite = -1;
		ctx->writeDone = true;
	}

	return (true);
}

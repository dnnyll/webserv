#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

#include "../inc/EventHandler.hpp"
#include <string>
#include <sys/types.h>
#include <vector>


struct	CgiInfo
{
	std::string scriptPath;			// full path to script/executable
	std::string interpreterPath;	// program to exec (e.g. /usr/bin/php-cgi)
	std::vector<std::string> env;	// KEY=VALUE strings for execve
};


class	CgiHandler : public EventHandler
{
	public:
		CgiHandler(const CgiInfo& info, std::string* outBuffer, const std::string& requestBody);
		~CgiHandler();

		// EventHandler interface
		void	handleRead();
		void	handleWrite();

		int		getFd() const;
		bool	isWritable() const;
		bool	isClosed() const;

	private:
		// CgiHandler(const CgiHandler&);
		// CgiHandler& operator=(const CgiHandler&);

		void	launchCgi(const CgiInfo& info);
		void	closeFd(int& fd);

	private:
		// Parent-side pipe descriptors
		int				_pipeInWrite;		// parent -> CGI stdin
		int				_pipeOutRead;		// CGI stdout -> parent

		// Child process
		pid_t			_pid;

		// ClientHandler response buffer (non-owning)
		std::string*	_outBuffer;

		// POST body
		std::string		_requestBody;
		size_t			_bodySent;

		// State
		bool			_stdinClosed;
		bool			_isClosed;
};

#endif

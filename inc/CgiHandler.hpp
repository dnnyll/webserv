#ifndef		CGIHANDLER_HPP
# define	CGIHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	<string>
#include	<sys/types.h>
#include	<ctime>

//TODO(jules): stock information in new structure during routing/response

 // CgiInfo.hpp
 #include <string>
 #include <vector>

 struct CgiInfo
 {
 	std::string scriptPath;			// full path to script/executable
 	std::string interpreterPath;	// program to exec (e.g. /usr/bin/php-cgi)
 	std::vector<std::string> env;	// KEY=VALUE strings for execve
 };


// TODO(danny): build cgihandler prototype

// class	CgiHandler : public	EventHandler
// {
// 	public:
// 		// stdin/stdout are connected inside this handler (it will create pipes + fork/exec)
// 		CgiHandler(const CgiInfo& info, std::string* outBuffer, const std::string& requestBody);
// 		~CgiHandler();

// 		// EventHandler interface
// 		void	handleRead();
// 		void	handleWrite();
// 		int		getFd() const;
// 		bool	isClosed() const;
// 		bool	isWritable() const;

// 	private:
// 		CgiHandler(const CgiHandler&);		// non-copyable (optional in C++98 style)
// 		CgiHandler& operator=(const CgiHandler&);

// 		void		forkAndExec(const CgiInfo& info);
// 		void		closeFd(int fd);

// 	private:
// 		// pipes (parent side)
// 		int			_pipeInWrite;		// parent writes -> CGI stdin
// 		int			_pipeOutRead;		// CGI stdout -> parent reads

// 		pid_t		_pid;

// 		std::string* _outBuffer;		// non-owning pointer to ClientHandler::_outBuffer

// 		// request body streaming into CGI stdin
// 		std::string	_requestBody;
// 		size_t		_bodySent;

// 		// CGI stdout accumulation
// 		std::string	_outputBuffer;

// 		bool		_isClosed;
// 		bool		_stdinClosed;

// 		// timeout (optional; you can implement later)
// 		time_t		_startTime;
// };

#endif

#ifndef		CGIHANDLER_HPP
# define	CGIHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	<string>
#include	<sys/types.h>
#include	<ctime>

// TODO(danny): build cgihandler prototype
class	CgiHandler : public EventHandler
{
	public:
		CgiHandler(int pipeIn, int pipeOut, pid_t pid, std::string *outBuffer, const std::string &requestBody);
	~CgiHandler();

		void	handleRead();
		void	handleWrite();
		int		getFd() const;
		bool	isClosed() const;
		bool	isWritable() const;

	private:
		int			_pipeIn;		// write end → send request body to script stdin
		int			_pipeOut;		// read end  → read script output
		pid_t		_pid;			// child process pid → for kill() and waitpid()
		std::string	*_outBuffer;	// pointer to ClientHandler._outBuffer
		std::string	_inputBuffer;	// request body waiting to be written to script
		std::string	_outputBuffer;	// accumulates script output as it arrives
		bool		_isClosed;		// true when script finished or timed out
		time_t		_startTime;		// for timeout detection
};

#endif

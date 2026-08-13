#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

#include	"../inc/EventHandler.hpp"
#include	<string>
#include	<vector>
#include	<sys/types.h>
#include	<ctime>

//TODO(jules): stock information in new structure during routing/response

struct CgiInfo
{
	std::string scriptPath;			// full path to script/executable
	std::string	workingDirectory;
 	std::string interpreterPath;	// program to exec (e.g. /usr/bin/php-cgi)
 	std::vector<std::string> env;	// KEY=VALUE strings for execve
	/*
		REQUEST_METHOD		this->_request.method
		QUERY_STRING		uri.find('?') -> substr
		CONTENT_LENGTH		this->_request.contentLength
		CONTENT_TYPE		this->_request.headers["Content-Type"]
		SCRIPT_NAME			uri.find('?') -> substr 
		SERVER_PROTOCOL		HTTP/1.1
		SERVER_NAME			this->_config.host
		SERVER_PORT			this->_config.port
		GATEWAY_INTERFACE	"CGI/1.1"
		REDIRECT_STATUS		=200
	 */
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
		bool	getClosed() const;

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
		bool			_getClosed;
};

#endif

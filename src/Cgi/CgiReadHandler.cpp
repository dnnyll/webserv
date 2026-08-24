#include	"../inc/Cgi/CgiReadHandler.hpp"
#include	"../inc/HttpResponse.hpp"
#include	"../inc/Router.hpp"
#include	<unistd.h>
#include	<sys/wait.h>
#include	<iostream>
#include	<sstream>
#include	<map>
#include	<cctype>

/*
	CGI output -> HTTP response.

	The script writes a header block followed by a blank line, then the
	body. We split the two, pull out the headers we control ourselves
	(Status, Location, Content-Length), forward the rest, and re-emit a
	well-formed HTTP/1.1 response with our own Content-Length so the
	client can do keep-alive.
*/

static std::string	toLower(const std::string &s)
{
	std::string	r = s;
	for (std::string::size_type i = 0; i < r.size(); ++i)
		r[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(r[i])));
	return (r);
}

static void	parseCgiHeaders(const std::string &output,
		std::map<std::string, std::string> &headers, std::string &body)
{
	std::string::size_type	sep = output.find("\r\n\r\n");
	std::string::size_type	bodyStart = 0;
	std::string				headerBlock;

	if (sep != std::string::npos)
	{
		headerBlock = output.substr(0, sep);
		bodyStart = sep + 4;
	}
	else
	{
		sep = output.find("\n\n");
		if (sep != std::string::npos)
		{
			headerBlock = output.substr(0, sep);
			bodyStart = sep + 2;
		}
	}
	body = output.substr(bodyStart);

	std::string::size_type	start = 0;
	while (start < headerBlock.size())
	{
		std::string::size_type	end = headerBlock.find('\n', start);
		if (end == std::string::npos)
			end = headerBlock.size();
		std::string	line = headerBlock.substr(start, end - start);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (!line.empty())
		{
			std::string::size_type	colon = line.find(':');
			if (colon != std::string::npos)
			{
				std::string	name = toLower(line.substr(0, colon));
				std::string	value = line.substr(colon + 1);
				std::string::size_type	first = value.find_first_not_of(" \t");
				if (first != std::string::npos)
					value = value.substr(first);
				headers[name] = value;
			}
		}
		start = end + 1;
	}
}

static std::string	buildCgiResponse(const std::string &output)
{
	std::map<std::string, std::string>	headers;
	std::string							body;
	std::string							statusCode = "200";
	std::string							statusMessage = "OK";

	parseCgiHeaders(output, headers, body);

	std::map<std::string, std::string>::iterator	it = headers.find("status");
	if (it != headers.end())
	{
		std::stringstream	ss(it->second);
		int					code;
		ss >> code;
		if (ss)
		{
			std::stringstream	codeStream;
			codeStream << code;
			statusCode = codeStream.str();
			ss >> std::ws;
			std::getline(ss, statusMessage);
			if (statusMessage.empty())
				statusMessage = "Unknown";
		}
		headers.erase(it);
	}

	//	a Location header without an explicit Status means a redirect
	it = headers.find("location");
	if (it != headers.end() && statusCode == "200")
	{
		statusCode = "302";
		statusMessage = "Found";
	}

	//	Content-Length is always recomputed from the actual body
	headers.erase("content-length");

	std::string	response = "HTTP/1.1 " + statusCode + " " + statusMessage + "\r\n";
	for (it = headers.begin(); it != headers.end(); ++it)
		response += it->first + ": " + it->second + "\r\n";
	std::stringstream	lengthStream;
	lengthStream << body.size();
	response += "Content-Length: " + lengthStream.str() + "\r\n\r\n";
	response += body;
	return (response);
}

void	CgiReadHandler::setErrorResponse(int code, const std::string &message) const
{
	if (!(_ctx->clientAlive && _ctx->clientAlive->alive && _ctx->outBuffer))
		return ;
	if (_ctx->config)
		*(_ctx->outBuffer) = Router::makeError(code, message, *_ctx->config).serialize();
	else
		*(_ctx->outBuffer) = HttpResponse::make(code, message).serialize();
}

CgiReadHandler::CgiReadHandler(CgiContext *ctx) : _ctx(ctx)
{
	_ctx->addRef();
}

CgiReadHandler::~CgiReadHandler()
{
	_ctx->release();
}

int	CgiReadHandler::getFd() const
{
	return (_ctx->pipeOutRead);
}

bool	CgiReadHandler::isWritable() const
{
	return (false);
}

bool	CgiReadHandler::getClosed() const
{
	if (_ctx->readDone)
		return (true);
	if (hasTimedOut())
	{
		std::cout << "[CGIREAD] timed out, killing CGI child" << std::endl;
		if (_ctx->pipeOutRead != -1)
		{
			close(_ctx->pipeOutRead);
			_ctx->pipeOutRead = -1;
		}
		if (_ctx->pid > 0)
			kill(_ctx->pid, SIGKILL);
		reapChild(true);
		setErrorResponse(504, "Gateway Timeout");
		_ctx->readDone = true;
		return (true);
	}
	return (false);
}

void	CgiReadHandler::setClosed()
{
	if (_ctx->readDone)
		return ;

	std::cout << "[CGIREAD] POLLERR, closing pipeOutRead" << std::endl;
	if (_ctx->pipeOutRead != -1)
	{
		close(_ctx->pipeOutRead);
		_ctx->pipeOutRead = -1;
	}
	if (_ctx->pid > 0)
		kill(_ctx->pid, SIGKILL);
	reapChild(true);
	setErrorResponse(500, "Internal Server Error");
	_ctx->readDone = true;
}

bool	CgiReadHandler::hasTimedOut() const
{
	return (time(NULL) - _ctx->startTime > TIMEOUT_SECONDS);
}

/*
	Reap the child and record its exit status. With block=false we wait
	with WNOHANG so this never blocks the reactor thread on the normal
	EOF path, where the child may have merely closed its stdout while
	still running. After a SIGKILL (timeout / POLLERR), block=true is
	used: the child is guaranteed to die promptly, and a blocking
	waitpid guarantees it is actually reaped instead of being lost to
	the non-blocking race.
*/
void	CgiReadHandler::reapChild(bool block) const
{
	if (_ctx->pid <= 0)
		return ;

	int	waitFlag;
	int	status;
	if (block)
		waitFlag = 0;
	else
		waitFlag = WNOHANG;
	if (waitpid(_ctx->pid, &status, waitFlag) > 0)
		_ctx->exitStatus = status;
	_ctx->pid = -1;
}

void	CgiReadHandler::handleRead()
{
	char	buf[4096];
	ssize_t	n = read(_ctx->pipeOutRead, buf, sizeof(buf));

	if (n > 0)
	{
		if (_ctx->clientAlive && _ctx->clientAlive->alive)
			_ctx->output.append(buf, n);
		//	if the client is gone, drop the bytes on the floor —
		//	we still need to keep draining the pipe so the child
		//	doesn't block on a full stdout buffer before exiting
		return ;
	}

	if (n < 0)
	{
		//	No errno inspection here: the subject forbids checking errno
		//	after a read/write. EAGAIN cannot occur because handleRead()
		//	is only dispatched when poll() reported POLLIN/POLLHUP; a
		//	transient error (e.g. EINTR) is retried on the next readiness
		//	event, and a persistent one surfaces as POLLERR/POLLNVAL and
		//	is handled by setClosed().
		return ;
	}

	//	n == 0 (EOF): the child closed stdout or exited — this side is done
	std::cout << "[CGIREAD] CGI output finished (fd=" << _ctx->pipeOutRead << ")" << std::endl;
	close(_ctx->pipeOutRead);
	_ctx->pipeOutRead = -1;
	reapChild(false);
	if (_ctx->clientAlive && _ctx->clientAlive->alive && _ctx->outBuffer)
	{
		if (_ctx->exitStatus != -1 && !WIFEXITED(_ctx->exitStatus))
			setErrorResponse(502, "Bad Gateway");
		else if (_ctx->exitStatus != -1 && WEXITSTATUS(_ctx->exitStatus) != 0)
			setErrorResponse(500, "Internal Server Error");
		else
			*(_ctx->outBuffer) = buildCgiResponse(_ctx->output);
	}
	_ctx->readDone = true;
}

void	CgiReadHandler::handleWrite()
{
	//	this handler is only ever polled for POLLIN; nothing to do here
}

#include	"../inc/CgiHandler.hpp"

CgiHandler::CgiHandler(const CgiInfo& info, std::string* outBuffer, const std::string& requestBody)
	:	_outBuffer(outBuffer),
		_requestBody(requestBody),
		_bodySent(0),
		_stdinClosed(false),
		_isClosed(false)
{
	launchCgi(info);
}

CgiHandler::~CgiHandler()
{
}

void	CgiHandler::launchCgi(const CgiInfo& info)
{
	(void)info;
}
// void	CgiHandler::closeFd(int& fd)
// {

// }

void CgiHandler::handleRead()
{
}

void CgiHandler::handleWrite()
{
}

int CgiHandler::getFd() const
{
	return (-1);
}

bool CgiHandler::isClosed() const
{
	return (false);
}

bool CgiHandler::isWritable() const
{
	return (false);
}

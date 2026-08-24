#include	"../inc/Cgi/CgiContext.hpp"
#include	<unistd.h>

CgiContext::CgiContext()
	: pid(-1),
	pipeInWrite(-1),
	pipeOutRead(-1),
	bodySent(0),
	outBuffer(NULL),
	clientAlive(NULL),
	config(NULL),
	writeDone(false),
	readDone(false),
	startTime(time(NULL)),
	exitStatus(-1),
	refCount(0)
{}

void	CgiContext::addRef()
{
	refCount++;
}

void	CgiContext::release()
{
	refCount--;
	if (refCount == 0)
	{
		if (pipeInWrite != -1)
			close(pipeInWrite);
		if (pipeOutRead != -1)
			close(pipeOutRead);
		if (clientAlive)
			clientAlive->release();
		delete this;
	}
}

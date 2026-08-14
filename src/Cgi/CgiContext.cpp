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

/*
	Once the last handler (write or read side, whichever finishes
	last) releases its reference, the context drops its reference on
	the shared clientAlive flag (the ClientHandler still holds one),
	then deletes itself. Pipes are expected to already be closed by
	whichever handler finished (see CgiWriteHandler/CgiReadHandler) —
	this is just a defensive double-check.
*/
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

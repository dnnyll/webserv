#include	"../inc/CgiContext.hpp"
#include	<unistd.h>

CgiContext::CgiContext()
	: pid(-1),
	pipeInWrite(-1),
	pipeOutRead(-1),
	bodySent(0),
	outBuffer(NULL),
	clientAlive(NULL),
	writeDone(false),
	readDone(false),
	startTime(time(NULL)),
	refCount(0)
{}

void	CgiContext::addRef()
{
	refCount++;
}

/*
	Once the last handler (write or read side, whichever finishes
	last) releases its reference, the context frees the shared
	clientAlive flag it was handed by ClientHandler, then deletes
	itself. Pipes are expected to already be closed by whichever
	handler finished (see CgiWriteHandler/CgiReadHandler) — this is
	just a defensive double-check.
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
		delete clientAlive;
		delete this;
	}
}

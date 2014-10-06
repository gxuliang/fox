
#include "base/sy_types.h"
#include "base/sy_debug.h"
#include "sy_thread.h"

void *ThreadBody(void *pdat)
{

	CThread *pThread = (CThread *)pdat;
	pThread->ThreadProc();

}

CThread::CThread()
{

}

CThread::~CThread()
{

}

bool CThread::CreateThread()
{
	int err = 0;
	err = pthread_create(&tid, NULL, ThreadBody, this);
	if(err != 0)
	{
		errorf("create thread failed!\n");
		return false;
	}
	infof("create thread ok!\n");
	return true;
}

bool CThread::CloseTread()
{
	pthread_cancel(tid);
}
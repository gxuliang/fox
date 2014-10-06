

#ifndef _CTHREAD_H_
#define _CTHREAD_H_

#include "base/sy_types.h"

class CThread
{
	//friend void* ThreadBody(void *pdat);
public:
	CThread();
	~CThread();
	bool CreateThread();
	bool CloseTread();

	virtual void ThreadProc() = 0;

private:
	pthread_t tid;

	
};

#endif

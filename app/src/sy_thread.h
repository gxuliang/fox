

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

	virtual void ThreadProc() = 0;


	
};

#endif

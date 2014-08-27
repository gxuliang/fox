/*
 * semaphore.h
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */

#ifndef _EX_SEMAPHORE_H_
#define _EX_SEMAPHORE_H_


#include <semaphore.h>



struct SemaphoreInternal;


class CSemaphore
{
public:
	CSemaphore(uint initialCount = 0);


	~CSemaphore();


	int Pend();


	int Post();

private:

	SemaphoreInternal	*m_internal;
};

#endif /* SEMAPHORE_H_ */

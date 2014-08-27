/*
 * semaphore.h
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_


#include <semaphore.h>

struct SemaphoreInternal
{
	sem_t *	m_sem;
};


#endif /* SEMAPHORE_H_ */

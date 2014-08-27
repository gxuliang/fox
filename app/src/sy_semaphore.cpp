/*
 * semaphore.cpp
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */

#include <assert.h>
#include "base/sy_semaphore.h"

#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


#include "sy_semaphore.h"

CSemaphore::CSemaphore(uint initialCount)
{
	m_internal = new SemaphoreInternal;


	int ret;
	m_internal->m_sem = (sem_t *)malloc(sizeof(sem_t));
	assert(m_internal->m_sem);
	ret=sem_init(m_internal->m_sem, 0, initialCount);
	assert(ret != -1);

}

CSemaphore::~CSemaphore()
{

	int ret;
	assert(m_internal->m_sem);
	ret = sem_destroy(m_internal->m_sem);
	assert(ret ==0);
	free(m_internal->m_sem);

	delete m_internal;
}


int CSemaphore::Pend()
{
	assert(m_internal->m_sem);
	int ret;
	while((ret = sem_wait(m_internal->m_sem)) != 0 && errno == EINTR);
	assert(ret == 0);
	return ret;
}


int CSemaphore::Post()
{
	assert(m_internal->m_sem);
	return sem_post(m_internal->m_sem);
}

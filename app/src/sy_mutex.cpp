/*
 * sy_mutex.cpp
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */



#include <assert.h>
#include "base/sy_mutex.h"

#ifdef WIN32
	#include <Windows.h>
#elif defined(__linux__)
	#include <stdlib.h>
	#include <pthread.h>
	/// ???? pthread_mutexattr_settype bad
	extern "C"	int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind);
#elif defined(__TCS__)
	#include <psos.h>
	//
	//save the m_handle pointer if root isn't entered,
	//and then fill immedietly after root entered.
	//the m_handle pointers are supposed to always valid.
	//
	#define MAX_UNRESOLVED_MUTEX_HANDLE 600
	static int psos_mutex_count = 0;
	static ulong *psos_mutex_array[MAX_UNRESOLVED_MUTEX_HANDLE];
	static char name[5] = "mute";
#else
	#error "Unknown Platform"
#endif

#include "sy_mutex.h"

CMutex::CMutex(Type nType)
{
	m_internal = new MutexInternal;
#ifdef WIN32
	m_internal->m_handle = CreateMutex(NULL, false, NULL);
	assert(m_internal->m_handle);

#elif defined(__linux__)
	int			ret;
	pthread_mutexattr_t attr;

	m_internal->m_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	assert(m_internal->m_lock);

	if(nType == mutexRecursive)
	{
		ret = pthread_mutexattr_init(&attr);
		assert(ret == 0);

		ret = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP);
		assert(ret == 0);

		ret = pthread_mutex_init(m_internal->m_lock, &attr);
		assert(ret == 0);
	}
	else
	{
		ret = pthread_mutex_init(m_internal->m_lock, NULL);
		assert(ret == 0);
	}


#elif defined(__TCS__)
	unsigned long ret;
	m_internal->m_id = 0;
	ret = mu_create(name, MU_LOCAL | MU_RECURSIVE | MU_PRIOR, 0, &m_internal->m_id);

	if(ret)
	{
		if(psos_mutex_count >= 0)
		{
			//save mutex m_internal->m_handle pointer, if root isn't entered.
			psos_mutex_array[psos_mutex_count++] = &m_internal->m_id;
			assert(psos_mutex_count < MAX_UNRESOLVED_MUTEX_HANDLE);
		}
		else
		{
			assert(m_internal->m_id);
		}
	}
	else
	{
		if(psos_mutex_count >= 0)
		{
			// once root entered, try to create unsolved mutex again.
			for(int i = 0; i < psos_mutex_count; i++)
			{
				ret = mu_create(name, MU_LOCAL | MU_RECURSIVE | MU_PRIOR, 0, psos_mutex_array[i]);
				assert(ret);
			}
			psos_mutex_count = -1;
		}
	}

#endif
}

CMutex::~CMutex()
{
#ifdef WIN32
	assert(m_internal->m_handle);
	CloseHandle(m_internal->m_handle);

#elif defined(__linux__)
	int		ret;
	assert(m_internal->m_lock);
	ret = pthread_mutex_destroy(m_internal->m_lock);
	assert(ret == 0);
	free(m_internal->m_lock);

#elif defined(__TCS__)
	unsigned long ret;
	assert(m_internal->m_id);
	ret = mu_delete(m_internal->m_id);
	assert(ret == 0);
#endif
	delete m_internal;
}

bool CMutex::Enter()
{
#ifdef WIN32
	assert(m_internal->m_handle);
	return (WaitForSingleObject(m_internal->m_handle, INFINITE) == WAIT_OBJECT_0);

#elif defined(__linux__)
	assert(m_internal->m_lock);
	return (pthread_mutex_lock(m_internal->m_lock) == 0);

#elif defined(__TCS__)
	assert(m_internal->m_id);
	return (mu_lock(m_internal->m_id, MU_WAIT, 0) == 0);

#endif
}

bool CMutex::Leave()
{
#ifdef WIN32
	assert(m_internal->m_handle);
	return (ReleaseMutex(m_internal->m_handle) != FALSE);

#elif defined(__linux__)
	assert(m_internal->m_lock);
	return (pthread_mutex_unlock(m_internal->m_lock) == 0);

#elif defined(__TCS__)
	assert(m_internal->m_id);
	return (mu_unlock(m_internal->m_id) == 0);

#endif
}


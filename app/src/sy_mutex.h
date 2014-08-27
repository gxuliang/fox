/*
 * mutex.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef MUTEX_H_
#define MUTEX_H_


struct MutexInternal
{
#ifdef WIN32
	void* m_handle;
#elif defined(__linux__)
	pthread_mutex_t *	m_lock;
#elif defined(__TCS__)
	unsigned long m_id;
#endif
};

#endif /* MUTEX_H_ */

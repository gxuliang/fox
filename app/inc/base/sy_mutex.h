/*
 * sy_mutex.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef EX_SY_MUTEX_H_
#define EX_SY_MUTEX_H_



#include "base/sy_types.h"

#ifdef __linux__
#include <pthread.h>
#endif

struct MutexInternal;

/// \class CMutex
/// \brief 多平台互斥量类
class CMutex
{
public:
	/// 互斥量类型
	enum Type
	{
		/// 快速互斥量，如果临界区已经被一个线程获取，其他线程或者该线程再次获取时，
		/// 调用线程直接被挂起。
		mutexFast = 0,

		/// 递归互斥量，已经获取临界区的线程可以多次获取，互斥量对象内部应有获取次数
		/// 计数。如果要完全释放临界区，一定要保证获取次数和释放次数一致。
		mutexRecursive,
	};

	/// 构造函数，会创建系统互斥量
	/// \param type 互斥量类型，取MutexType类型的值
	CMutex(Type type = mutexFast);

	/// 析构函数，会销毁系统互斥量
	~CMutex();

	/// 进入临界区。
	/// \return 操作是否成功
	bool Enter();

	/// 离开临界区。
	/// \return 操作是否成功
	bool Leave();

private:

	MutexInternal *m_internal;
};

#endif /* SY_MUTEX_H_ */

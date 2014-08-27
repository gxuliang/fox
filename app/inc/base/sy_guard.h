/*
 * sy_guard.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef SY_GUARD_H_
#define SY_GUARD_H_


#include "base/sy_mutex.h"

class CMutex;

/// \class CGuard
/// \brief 守护者类。
///
/// 构造时自动调用CMutex::Enter，析构时自动调用CMutex::Leave，避免不
/// 配对的情况发生。示例如下：
/// \code
/// CMutex mutex;
/// int needProtect = 0;
/// void testAdd()
/// {
/// there:
/// 	if(needProtect % 2) // not protected
/// 	{
/// 		needProtect++;
/// 	}
/// 	else
/// 	{
/// 		return;
/// 	}
///
/// 	CGuard guard(mutex);
/// 	needProtect++;
/// 	goto there;	// will also bring ~CGuard(), thus mutex.Leave()
/// }
/// void testSub()
/// {
///     needProtect--; // not protected
///
/// 	CGuard guard(mutex);
/// 	needProtect--;
/// }
/// \endcode

class CGuard
{
public:
	/// 构造函数，自动调用CMutex::Enter
	/// \param mutex 互斥量引用，CGuard并不创建互斥量。
	inline CGuard(CMutex& mutex)
		:m_mutex(mutex)
	{
		m_mutex.Enter();
	};

	/// 析构函数，自动调用CMutex::Leave
	inline ~CGuard()
	{
		m_mutex.Leave();
	};

private:
	CMutex &m_mutex;	///< 需要自动调用的互斥量引用
};



#endif /* SY_GUARD_H_ */

/*
 * singletion.h
 *
 *  Created on: 2013-3-19
 *      Author: xul
 */

#ifndef SINGLETION_H_
#define SINGLETION_H_

#include <stdio.h>


#define PATTERN_SINGLETON_DECLARE(classname)	\
public:											\
static classname * instance();					\
private:										\
classname();									\
~classname();									\


#define PATTERN_SINGLETON_IMPLEMENT(classname)	\
classname * classname::instance()		        \
{												\
	static classname * _instance = NULL;		\
	if( NULL == _instance)						\
	{											\
		_instance = new classname;				\
	}											\
	return _instance;							\
}


#endif /* SINGLETION_H_ */

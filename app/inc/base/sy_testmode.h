/*
 * sy_testmode.h
 *
 *  Created on: 2014-10-30
 *      Author: xul
 */

#ifndef _EX_TESTMODE_H_
#define _EX_TESTMODE_H_

#include "base/sy_singletion.h"


class ITestMode
{
public:
	static ITestMode *instance();
	virtual ~ITestMode(){};
	virtual void run(const char* path) = 0;
	virtual void save(char* buf, int len)=0;
	
};


#endif


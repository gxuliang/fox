/*
 * sy_testmode.h
 *
 *  Created on: 2014-10-30
 *      Author: xul
 */

#ifndef _TESTMODE_H_
#define _TESTMODE_H_

#include "base/sy_testmode.h"

class CTestMode : public ITestMode
{
public:
	PATTERN_SINGLETON_DECLARE(CTestMode)
public:
	void run(const char* path);
	void save(char* buf, int len);

	/* data */
private:
	int mfd;
	int flag;
};

#endif

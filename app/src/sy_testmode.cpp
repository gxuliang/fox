/*
 * sy_testmode.cpp
 *
 *  Created on: 2014-10-30
 *      Author: xul
 */

#include "base/sy_types.h"
#include "NetService/sy_netServ.h"

#include "sy_testmode.h"
#include "base/sy_debug.h"


PATTERN_SINGLETON_IMPLEMENT(CTestMode)


CTestMode::CTestMode()
{
	flag = 0;
}
	
CTestMode::~CTestMode()
{

}
void CTestMode::run(const char* path)
{
	sleep(5);
	errorf("============sned start================\n");
	int fd = ::open(path, O_RDONLY, S_IWUSR);
	if(fd <= 0)
		return;

	
	flag = 1;

	char buf[2000]="";
	int len;
	while((len = read(fd, buf, 2000)) > 0)
	{
		INetServ::instance()->write(buf, len);
		fatalf("AAA============write len = %d================\n", len);
		//usleep(200*1000);
	}
	errorf("============sned end================\n");
	close(fd);
}
void CTestMode::save(char* buf, int len)
{
	tracepoint();
	if(flag == 0)
		return;

	tracepoint();
	mfd =  ::open("check.txt",O_WRONLY | O_CREAT | O_APPEND, S_IWUSR);
	if(mfd <= 0)
		return;

	int ret = 0, cnt = 0;
	warnf("BBB============write len = %d================\n", len);

	char* p;
	while(len > 0)
	{
		p = &buf[cnt];
		ret = ::write(mfd, p, len);
		errorf("CCC============write len = %d================\n", ret);
		len = len - ret;
		cnt = cnt + ret;
	}
	::close(mfd);

}

ITestMode *ITestMode::instance()
{
	return CTestMode::instance();
}
/*
 * sy_netService.h
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#ifndef _SY_NETSERVICE_H_
#define _SY_NETSERVICE_H_

#include "json/value.h"
#include "json/writer.h"
#include "json/reader.h"

#include "base/sy_types.h"
#include "base/sy_mutex.h"
#include "base/sy_singletion.h"
#include "base/sy_net.h"
#include "NetService/sy_netService.h"
#include "Manager/sy_configManager.h"
#include "sy_thread.h"


class CNetService : public INetService, public CThread, public ISetConfig
{
public:
	PATTERN_SINGLETON_DECLARE(CNetService)

	bool setConfig(const char* name,const CConfigTable& table);//设置后自动生效
	bool getState(CConfigTable& table);//获取连接状态
	bool reg(IPrinter* p);
	int write(const char*, int);


private:
	bool start();
	bool stop();
	bool restart();
	void ThreadProc();
	std::string ipaddr;
	ushort port;
	MyClient* sock;
	bool loopflag;
	CSemaphore sem;
	IPrinter* mPrinter;
};

#endif

/*
 * sy_netServ.h
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
#include "NetService/sy_netServ.h"
#include "Manager/sy_configManager.h"
#include "sy_thread.h"

#define HEAD_LEN 6

class CProtocol;


class CNetService : public INetService, public CThread, public ISetConfig
{
public:
	PATTERN_SINGLETON_DECLARE(CNetService)

	bool setConfig(const char* name,const CConfigTable& table);//设置后自动生效
	bool getState(CConfigTable& table);//获取连接状态
	bool reg(IPrinter* p);
	int write(const char*, int);
	bool restart();


private:
	bool start();
	bool stop();
	void ThreadProc();
	std::string ipaddr;
	ushort port;
	MyClient* sock;
	bool loopflag;
	CSemaphore sem;
	IPrinter* mPrinter;
};


class CCtlNetService : public ICtlNetService, public CThread, public ISetConfig
{
public:
	PATTERN_SINGLETON_DECLARE(CCtlNetService)

	bool setConfig(const char* name,const CConfigTable& table);//设置后自动生效
	bool getState(CConfigTable& table);//获取连接状态
	bool reg(IPrinter* p);
	int write(const char*, int);
	bool restart();


private:
	bool start();
	bool stop();
	void ThreadProc();
	std::string ipaddr;
	ushort port;
	MyClient* sock;
	bool loopflag;
	CSemaphore sem;
	CProtocol *mprotocol;
};

class CProtocol
{
public:
	CProtocol();
	bool deal(ICtlNetService* p, char* buf);
	bool sendCmd(ICtlNetService* p, uchar type);
	bool alarm;
private:
	bool sendCmdRespone(ICtlNetService* p, uchar type);
	bool getMAC(ICtlNetService* p, uchar type);
	bool enableAlarm(ICtlNetService* p, uchar type);
	bool disableAlarm(ICtlNetService* p, uchar type);
	bool getSW(ICtlNetService* p, uchar type);
	bool getHW(ICtlNetService* p, uchar type);
	bool getHeart(ICtlNetService* p, uchar type);
	bool resetHW(ICtlNetService* p, uchar type);
	char head[HEAD_LEN];
	char* psendbuf;
};

#endif

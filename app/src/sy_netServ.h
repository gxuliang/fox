/*
 * sy_netService.h
 *
 *  Created on: 2014-10-20
 *      Author: xul
 */

#ifndef _SY_NETSERV_H_
#define _SY_NETSERV_H_

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

struct IPINFO
{
	std::string ipaddr;
	ushort port[2];	
};

class CNetServ : public INetServ, public CThread, public ISetConfig
{
public:
	PATTERN_SINGLETON_DECLARE(CNetServ)

	bool setConfig(const char* name,const CConfigTable& table);//设置后自动生效
	bool reg(IPrinter* p);
	int write(const char*, int);
	int write2(const char*, int);
	bool restart();
	void conn_clear();
	bool getState();



private:
	void ThreadProc();
	bool stop();
	bool start();

	bool loopflag;
	IPINFO ipinfo[2];
	int count, timeout, con_reset;
	IPrinter* mPrinter;
	char* buf;
	MyClient* sock1,*sock2;
	CProtocol *mprotocol;
	int fd[2];
	bool startflag;



};

class CProtocol
{
public:
	CProtocol();
	bool deal(INetServ* p, char* buf);
	bool sendCmd(INetServ* p, uchar type);
	bool alarm;
private:
	bool sendCmdRespone(INetServ* p, uchar type);
	bool getMAC(INetServ* p, uchar type);
	bool enableAlarm(INetServ* p, uchar type);
	bool disableAlarm(INetServ* p, uchar type);
	bool getSW(INetServ* p, uchar type);
	bool getHW(INetServ* p, uchar type);
	bool getHeart(INetServ* p, uchar type);
	bool resetHW(INetServ* p, uchar type);
	char head[HEAD_LEN];
	char* psendbuf;
};


#endif

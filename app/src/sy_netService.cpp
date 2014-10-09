/*
 * sy_netService.cpp
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#include "base/sy_debug.h"
#include "base/sy_semaphore.h"
#include "Manager/sy_file.h"
#include "base/sy_guard.h"
#include "sy_printer.h"
#include "sy_netService.h"
#include "sy_printer.h"
#include "Device/sy_device.h"



extern IPrinter *gPrintOut;

PATTERN_SINGLETON_IMPLEMENT(CNetService)

CNetService::~CNetService()
{
	
}

CNetService::CNetService()
{
	loopflag = false;
	//mPrinter = NULL;
	infof("000000000000000[%p]\n", this);
	strncpy(this->str, "NetService", sizeof(str));
	IConfigManager::instance()->reg(this);
	//warnf("=======++===%p======\n", gPrintOut);
	//this->reg(gPrintOut);
}

bool CNetService::reg(IPrinter* p)
{
	mPrinter = p;
	warnf("==========%p===%p===\n", mPrinter,p);
	return true;
}

bool CNetService::setConfig(const char* name, const CConfigTable& table) 
{
	this->reg(gPrintOut);
	ipaddr = table["ipaddr"].asString();
	//port = table["port"].asUInt();
	port = 1001;
	strncpy(this->str, name, sizeof(str));
	infof("[%p],name is [%s], ip = [%s], port = %d\n", this, this->str, ipaddr.c_str(), port);
	restart();
	return true;
}

bool CNetService::getState(CConfigTable& table)
{

	return true;
}


bool CNetService::stop()
{
	loopflag = false;
	if(sock != NULL)
	{
		delete sock;
		sock = NULL;
	}



	infof("CNetService::stopping...\n");
	this->CloseTread();
	infof("CNetService::stop ok!\n");
	return true;
}

bool CNetService::start()
{
	loopflag = true;
	infof("CNetService::starting...\n");
	this->CreateThread();
	infof("CNetService::start ok\n");
	return true;
}

bool CNetService::restart()
{
	if(loopflag)
	{
		stop();
	}

	return start();

}

#define BUF_MAX (1024*2)
void CNetService::ThreadProc()
{
	sock = new MyClient();
	char * buf = new char[BUF_MAX];
	int ctl_state = 0;
	while(loopflag)
	{
		//tracepoint();
		if(sock->conn_flag == false)
		{	
			if(sock->connect(ipaddr.c_str(), port) == false)
			{
				//ctl_state = (ctl_state + 1) & 0x01;
				IDevice::instance()->setLed(IDevice::LED_CONN, 2);
				sock->perror("CNetService::connect");
				sleep(2);
				continue;
			}
		}

		IDevice::instance()->setLed(IDevice::LED_CONN, 0);

		int len = sock->read(buf, BUF_MAX);
		if(len > 0)
		{
			infof("read network data len = %d\n", len);
			debugf("[%c]----%p--%p\n", buf[0],mPrinter,gPrintOut);

			while(mPrinter->put(buf, len) == false)
			{
				errorf("buf is full,try it again after 1sc...\n");
				sleep(1);
			}
		}
		else if(len <= 0)
		{	
			if(loopflag == false)
			{
				tracepoint();
				infof("CNetService::it will be over1!\n");
				tracepoint();
				break;
			}
			errorf("net ill, reconnect!\n");
			sock->close();
			continue;
		}
		else if(0)//if(len == 0)
		{
			tracepoint();
			if(loopflag == false)
			{
				tracepoint();
				infof("CNetService::it will be over2!\n");
				tracepoint();
				break;
			}
			continue;
		}
		
		

	}
	tracepoint();
	if(sock != NULL)
	{
		delete sock;
		sock == NULL;
	}
	delete []buf;
	//sem.Post();
}

int CNetService::write(const char* buf, int len)
{
	if(sock == NULL)
		return -1;

	return sock->write(buf, len);
}


INetService *INetService::instance()
{
	//infof("[%p]\n", CNetService::instance());


	return CNetService::instance();
}

////////////////////////////
PATTERN_SINGLETON_IMPLEMENT(CCtlNetService)

CCtlNetService::~CCtlNetService()
{
	
}

CCtlNetService::CCtlNetService()
{
	loopflag = false;
	tracepoint();
	strncpy(this->str, "NetService", sizeof(str));
	IConfigManager::instance()->reg(this);
	tracepoint();
	mprotocol = new CProtocol();
}

bool CCtlNetService::setConfig(const char* name, const CConfigTable& table) 
{
	ipaddr = table["ipaddr"].asString();
	//port = table["port"].asUInt();
	port  = 1002;
	strncpy(this->str, name, sizeof(str));
	infof("[%p],name is [%s], ip = [%s], port = %d\n", this, this->str, ipaddr.c_str(), port);
	restart();
	return true;
}

bool CCtlNetService::getState(CConfigTable& table)
{

	return true;
}

bool CCtlNetService::stop()
{
	loopflag = false;
	if(sock != NULL)
	{
		
		delete sock;
		sock = NULL;
	}
	infof("CCtlNetService::stopping...\n");
	this->CloseTread();
	infof("CCtlNetService::stop ok!\n");
	return true;
}

bool CCtlNetService::start()
{
	loopflag = true;
	infof("CCtlNetService::starting...\n");
	this->CreateThread();
	infof("CCtlNetService::start ok\n");
	return true;
}

bool CCtlNetService::restart()
{
	if(loopflag)
	{
		stop();
	}

	return start();

}

void CCtlNetService::ThreadProc()
{
	sock = new MyClient();
	char * buf = new char[BUF_MAX];
	while(loopflag)
	{
		//tracepoint();
		if(sock->conn_flag == false)
		{	
			if(sock->connect(ipaddr.c_str(), port) == false)
			{
				IDevice::instance()->setLed(IDevice::LED_ALARM, 1);
				sock->perror("CCtlNetService::connect");
				sleep(2);
				continue;
			}
		}

			IDevice::instance()->setLed(IDevice::LED_ALARM, 0);
			int len = sock->read(buf, BUF_MAX);
			if(len > 0)
			{
				infof("read network data len = %d\n", len);
				mprotocol->deal(this, buf);
			}
			else if(len <= 0)
			{
				if(loopflag == false)
				{
					infof("CCtlNetService::it will be over1!\n");
					break;
				}

				errorf("net ill, reconnect!\n");
				sock->close();
				continue;
			}
			else if(0)//(len == 0)
			{
				tracepoint();

				if(loopflag == false)
				{
					infof("CCtlNetService::it will be over2!\n");
					break;
				}
				continue;
			}
		

	}
	tracepoint();
	if(sock != NULL)
	{
		delete sock;
		sock == NULL;
	}
	delete []buf;
	//sem.Post();
}

int CCtlNetService::write(const char* buf, int len)
{
	if(sock == NULL)
		return -1;

	return sock->write(buf, len);
}

ICtlNetService *ICtlNetService::instance()
{
	//infof("[%p]\n", CCtlNetService::instance());


	return CCtlNetService::instance();
}
//////////////

CProtocol::CProtocol()
{
	head[0] = 0xAA;
	head[1] = 0xAA;
	head[2] = 0x55;
	head[3] = 0x55;
	head[4] = 0xAA;
	head[5] = 0x55;

	psendbuf = new char[1400];
	memcpy(psendbuf, head, HEAD_LEN);
}

bool CProtocol::deal(ICtlNetService* p, char* buf)
{
	if(memcmp(buf, head, HEAD_LEN) !=0)
		return false;

	uchar len = buf[HEAD_LEN];

	uchar type = buf[HEAD_LEN+1];

	uchar cmd = buf[HEAD_LEN+2];

	infof("---cmd=%x, len=%d,type=%x------\n", cmd, len, type);
	switch(cmd)
	{
		case 0x00:
			return getMAC(p, type);
		case 0x03:
			return getHeart(p, type);
		case 0x06:
			return enableAlarm(p, type);
		case 0x07: 
			return disableAlarm(p, type);
		case 0x08:
			return getSW(p, type);
		case 0x09:
			return getHW(p, type);
		case 0xFD:
			return resetHW(p, type);
		default:
			return false;
	}

}
bool CProtocol::enableAlarm(ICtlNetService* p, uchar type)
{
	if(type == 0x00)
	{
		infof("CProtocol::enableAlarm\n");
		IDevice::instance()->setLed(IDevice::LED_ALARM, 2);
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x06;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write(psendbuf, len);
	}
	
	return true;
}
bool CProtocol::disableAlarm(ICtlNetService* p, uchar type)
{
	if(type == 0x00)
	{
		infof("CProtocol::disableAlarm\n");
		IDevice::instance()->setLed(IDevice::LED_ALARM, 0);
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x07;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write(psendbuf, len);
	}
	
	return true;
}
bool CProtocol::getMAC(ICtlNetService* p, uchar type)
{
	infof("CProtocol::getMAC\n");

	char mac[6] = {0x00,0x11,0x22,0x33,0x44,0x55};
	IDevice::instance()->getMAC(mac);
	if(type == 0x00)
	{
		psendbuf[HEAD_LEN] = 8;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x00;
		memcpy(&psendbuf[HEAD_LEN+3], mac, 6);
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write(psendbuf, len);
	}
	
	return true;
}

bool CProtocol::getSW(ICtlNetService* p, uchar type)
{
	infof("CProtocol::getSW\n");

	CConfigTable base;

	IConfigManager::instance()->getConfig("base", base);

	std::string sw = base["sw"].asString();

	psendbuf[HEAD_LEN] = sw.length() + 2;
	psendbuf[HEAD_LEN+1] = 0x01;
	psendbuf[HEAD_LEN+2] = 0x08;
	memcpy(&psendbuf[HEAD_LEN+3], sw.c_str(), sw.length());
	int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
	p->write(psendbuf, len);

	return true;

}

bool CProtocol::getHW(ICtlNetService* p, uchar type)
{
	infof("CProtocol::getHW\n");

	CConfigTable base;

	IConfigManager::instance()->getConfig("base", base);

	std::string hw = base["hw"].asString();

	psendbuf[HEAD_LEN] = hw.length() + 2;
	psendbuf[HEAD_LEN+1] = 0x01;
	psendbuf[HEAD_LEN+2] = 0x09;
	memcpy(&psendbuf[HEAD_LEN+3], hw.c_str(), hw.length());
	int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
	p->write(psendbuf, len);

	return true;

}

bool CProtocol::getHeart(ICtlNetService* p, uchar type)
{
	infof("CProtocol::getHeart\n");
	if(type == 0x00)
	{
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x03;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write(psendbuf, len);
	}
	
	return true;
}

bool CProtocol::resetHW(ICtlNetService* p, uchar type)
{
	infof("CProtocol::resetHW\n");
	if(type == 0x00)
	{
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0xFD;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write(psendbuf, len);
	}

	system("reboot");
	
	return true;
}

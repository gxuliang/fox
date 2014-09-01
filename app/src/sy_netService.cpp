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



extern IPrinter *gPrintOut;

PATTERN_SINGLETON_IMPLEMENT(CNetService)

CNetService::~CNetService()
{
	
}

CNetService::CNetService()
{
	loopflag = false;
	mPrinter = NULL;
	infof("000000000000000[%p]\n", this);
	strncpy(this->str, "abcd", sizeof(str));
	IConfigManager::instance()->reg(this);
	this->reg(gPrintOut);
}

bool CNetService::reg(IPrinter* p)
{
	mPrinter = p;
	return true;
}

bool CNetService::setConfig(const char* name, const CConfigTable& table) 
{
	ipaddr = table["ipaddr"].asString();
	port = table["port"].asUInt();
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
	sem.Pend();	
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
	while(loopflag)
	{
		if(sock->conn_flag == false)
		{	
			if(sock->connect(ipaddr.c_str(), port) == false)
			{
				sock->perror("connect");
				sleep(2);
				continue;
			}
		}

			int len = sock->read(buf, BUF_MAX);
			if(len > 0)
				infof("read network data len = %d\n", len);
			if(len < 0)
			{
				errorf("net ill, reconnect!\n");
				sock->close();
				continue;
			}
			else if(len == 0)
			{
				continue;
			}
			while(mPrinter->put(buf, len) == false)
			{
				errorf("buf is full,try it again after 1sc...\n");
				sleep(1);
			}
		

	}
	tracepoint();
	if(sock != NULL)
	{
		delete sock;
		sock == NULL;
	}
	delete []buf;
	sem.Post();
}

int CNetService::write(const char* buf, int len)
{
	if(sock == NULL)
		return -1;

	return sock->write(buf, len);
}


INetService *INetService::instance()
{
	infof("[%p]\n", CNetService::instance());


	return CNetService::instance();
}


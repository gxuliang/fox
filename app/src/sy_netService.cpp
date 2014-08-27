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



PATTERN_SINGLETON_IMPLEMENT(CNetService)

CNetService::~CNetService()
{
	
}

CNetService::CNetService()
{
	loopflag = false;
	mPrinter = NULL;
	IConfigManager::instance()->reg(this);
}

bool CNetService::reg(IPrinter* p)
{
	mPrinter = p;
	return true;
}

bool CNetService::setConfig(const char* name, CConfigTable& table) 
{
	ipaddr = table["ipaddr"].asString();
	port = table["port"].asUInt();
	strncpy(str, name, sizeof(str));
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
				sleep(1);
				continue;
			}

			int len = sock->read(buf, BUF_MAX);
			infof("read network data len = %d\n", len);
			while(mPrinter->put(buf, len) == false)
			{
				errorf("buf is full,try it again after 1sc...\n");
				sleep(1);
			}
		}

	}

	delete sock;
	delete []buf;
	sock == NULL;
	sem.Post();
}

INetService *INetService::instance()
{
	return CNetService::instance();
}


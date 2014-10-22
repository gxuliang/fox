/*
 * sy_netServ.cpp
 *
 *  Created on: 2014-10-20
 *      Author: xul
 */

#include "base/sy_debug.h"
#include "base/sy_semaphore.h"
#include "Manager/sy_file.h"
#include "base/sy_guard.h"
#include "sy_printer.h"
#include "sy_netServ.h"
#include "sy_printer.h"
#include "Device/sy_device.h"

#define BUF_MAX (1024*2)


extern IPrinter *gPrintOut;

PATTERN_SINGLETON_IMPLEMENT(CNetServ)

CNetServ::~CNetServ()
{
	delete []buf;
}

CNetServ::CNetServ()
{
	loopflag = false;
	strncpy(this->str, "ServerInfo", sizeof(str));
	IConfigManager::instance()->reg(this);
	buf = new char[BUF_MAX];
	mprotocol = new CProtocol();
	pipe(fd);
	

}

bool CNetServ::reg(IPrinter* p)
{
	mPrinter = p;
	warnf("==========%p===%p===\n", mPrinter,p);
	return true;
}


bool CNetServ::getState()
{
	if(sock2 != NULL && sock2->conn_flag == true)
		return true;
	else
		return false;
}

bool CNetServ::setConfig(const char* name, const CConfigTable& table) 
{
	this->reg(gPrintOut);
	ipinfo[0].ipaddr = table["ipinfo"][0u]["ipaddr"].asString();
	ipinfo[0].port[0] = table["ipinfo"][0u]["port"][0u].asInt();
	ipinfo[0].port[1] = table["ipinfo"][0u]["port"][1].asInt();

	ipinfo[1].ipaddr = table["ipinfo"][1]["ipaddr"].asString();
	ipinfo[1].port[0] = table["ipinfo"][1]["port"][0u].asInt();
	ipinfo[1].port[1] = table["ipinfo"][1]["port"][1].asInt();
	
	count = table["count"].asInt();
	timeout = table["timeout"].asInt();
	infof("============================================\n");
	infof("ip[%s],port[%d][%d],ip[%s],port[%d][%d],count:%d,timeout:%d\n", 
		ipinfo[0].ipaddr.c_str(),ipinfo[0].port[0],ipinfo[0].port[1],
		ipinfo[1].ipaddr.c_str(),ipinfo[1].port[0],ipinfo[1].port[1],
		count,timeout);
	infof("============================================\n");
	restart();

	return true;
}

void CNetServ::ThreadProc()
{
	
	sock1 = new MyClient();//控制
	sock2 = new MyClient();//数据

	int i = 0,cnt = 0;
	fd_set readfds;
	struct timeval overtime;



	IDevice::instance()->setLed(IDevice::LED_CONN, 2);
	IDevice::instance()->setLed(IDevice::LED_ALARM, 3, 1, 10);//1=0.5秒，10=5秒

	while(loopflag)
	{
		if(sock1->conn_flag == false)
		{
			if(sock1->connect(ipinfo[i].ipaddr.c_str(), ipinfo[i].port[1]) == false)
			{
				infof("======================%s====\n", ipinfo[i].ipaddr.c_str());
				IDevice::instance()->setLed(IDevice::LED_ALARM, 3, 1, 10);//1=0.5秒，10=5秒
				sock1->perror("CNetServ::connect");
				sleep(timeout);
				cnt++;
				if(cnt > count)
				{
					cnt = 0;
					i = (i+1) & 0x01;
				}
				
				continue;
				
			}
		}

		if(mprotocol->alarm == false)
			IDevice::instance()->setLed(IDevice::LED_ALARM, 0);
		infof("sock1->fd = %d==============98========%s====\n", sock1->fd,ipinfo[0].ipaddr.c_str());

		if(sock2->conn_flag == false)
		{
			infof("sock2->fd = %d==============112=======%s=====\n", sock2->fd, ipinfo[1].ipaddr.c_str());
			if(sock2->connect(ipinfo[i].ipaddr.c_str(), ipinfo[i].port[0]) == false)
			{//这个错误一般不会发生
				IDevice::instance()->setLed(IDevice::LED_CONN, 2);
				errorf("serv[%s] is port[%d] error!\n", ipinfo[i].ipaddr.c_str(), ipinfo[i].port[0]);
				sock1->close();//注意是关闭sock1
				sleep(timeout);
				continue;
			}
		}

		IDevice::instance()->setLed(IDevice::LED_CONN, 0);
		infof("sock2->fd = %d==============113============\n", sock2->fd);


		FD_ZERO(&readfds);
		FD_SET(sock1->fd, &readfds);
		FD_SET(sock2->fd, &readfds);
		FD_SET(fd[0], &readfds);
		overtime.tv_sec = 3;
		overtime.tv_usec = 0;

		int max_fd = (sock1->fd > sock2->fd)?sock1->fd:sock2->fd;
		infof("----------------%d+++++++++\n",overtime.tv_sec);
		int ret = ::select(max_fd+1, &readfds, NULL, NULL, &overtime);
		if(ret == 0)
		{//3秒超时到了，发送心跳包
			tracepoint();
			infof("++++++++++++++++%d+++++++++\n",overtime.tv_sec);
			if(mprotocol->sendCmd(this, 0xFE) == false)
			{
				errorf("net ill, reconnect!\n");
				sock1->close();
				sock2->close();
				con_reset = 0;
				continue;
			}	
			con_reset ++;
			if(con_reset >= 3)
			{
				tracepoint();
				errorf("something error!\n");		
				con_reset = 0;
				sock1->close();
				sock2->close();
				continue;
			}
		}
		else if(ret < 0)
		{
			tracepoint();
			errorf("something error!\n");
			sock1->close();
			sock2->close();
			continue;
		}
		else
		{
			if(FD_ISSET(fd[0], &readfds) != 0)
			{
				int len = read(fd[0], buf, BUF_MAX);
				tracepoint();
				errorf("param changed, close socket!\n");
				sock1->close();
				sock2->close();
				continue;
			}
			else if(FD_ISSET(sock1->fd, &readfds) != 0)
			{
				int len = sock1->read(buf, BUF_MAX);
				if(len > 0)
				{
					infof("read network data len = %d\n", len);
					mprotocol->deal(this, buf);		
				}
				else
				{
					tracepoint();
					errorf("something error!\n");
					sock1->close();
					sock2->close();
					continue;
				}
			}
			else if(FD_ISSET(sock2->fd, &readfds) != 0)
			{
				int len = sock2->read(buf, BUF_MAX);
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
				else
				{
					tracepoint();
					errorf("something error!\n");
					sock1->close();
					sock2->close();
					continue;
				}
			}
		}

	}
}

bool CNetServ::stop()
{
	loopflag = false;
	/*
	if(sock1 != NULL)
	{
		delete sock1;
		sock1 = NULL;
	}
	if(sock2 != NULL)
	{
		delete sock2;
		sock2 = NULL;
	}
	*/

	infof("CNetServ::stopping...\n");
	this->CloseTread();
	infof("CNetServ::stop ok!\n");
	return true;
}

bool CNetServ::start()
{
	loopflag = true;
	infof("CNetServ::starting...\n");
	this->CreateThread();
	infof("CNetServ::start ok\n");
	return true;
}

bool CNetServ::restart()
{
	if(loopflag)
	{
		stop();
	}

	return start();

}

void CNetServ::conn_clear()
{
	con_reset = 0;
}
int CNetServ::write(const char* buf, int len)
{
	if(sock2 == NULL)
		return -1;

	return sock2->write(buf, len);
}

int CNetServ::write2(const char* buf, int len)
{
	if(sock1 == NULL)
		return -1;

	return sock1->write(buf, len);
}

INetServ *INetServ::instance()
{
	return CNetServ::instance();
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
	alarm = false;
}

bool CProtocol::deal(INetServ* p, char* buf)
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
		case 0x04://设备发送心跳包
			return sendCmdRespone(p,type);
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
		case 0xFE:
			p->conn_clear();
			return sendCmdRespone(p,type);
		default:
			warnf("unknow cmd %x\n", cmd);
			return false;
	}

}
bool CProtocol::sendCmdRespone(INetServ* p, uchar type)
{
	warnf("the type %x\n", type);
	return false;
}
bool CProtocol::sendCmd(INetServ* p, uchar type)
{
	infof("CProtocol::sendCmd\n");
	psendbuf[HEAD_LEN] = 2;
	psendbuf[HEAD_LEN+1] = 0x00;
	psendbuf[HEAD_LEN+2] = type;
	int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
	int ret = p->write2(psendbuf, len);
	infof("====len=%d=====ret = %d====%p====\n", len, ret, p);

	if(ret <= 0)
		return false;
	else
		return true;
}
bool CProtocol::enableAlarm(INetServ* p, uchar type)
{
	if(type == 0x00)
	{
		infof("CProtocol::enableAlarm\n");
		IDevice::instance()->setLed(IDevice::LED_ALARM, 1);
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x06;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write2(psendbuf, len);
		alarm = true;

	}
	
	return true;
}
bool CProtocol::disableAlarm(INetServ* p, uchar type)
{
	if(type == 0x00)
	{
		infof("CProtocol::disableAlarm\n");
		IDevice::instance()->setLed(IDevice::LED_ALARM, 0);
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x07;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write2(psendbuf, len);
		alarm = false;

	}
	
	return true;
}
bool CProtocol::getMAC(INetServ* p, uchar type)
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
		p->write2(psendbuf, len);
	}
	
	return true;
}

bool CProtocol::getSW(INetServ* p, uchar type)
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
	p->write2(psendbuf, len);

	return true;

}

bool CProtocol::getHW(INetServ* p, uchar type)
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
	p->write2(psendbuf, len);

	return true;

}

bool CProtocol::getHeart(INetServ* p, uchar type)
{
	infof("CProtocol::getHeart\n");
	if(type == 0x00)
	{
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0x03;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write2(psendbuf, len);
	}
	
	return true;
}

bool CProtocol::resetHW(INetServ* p, uchar type)
{
	infof("CProtocol::resetHW\n");
	if(type == 0x00)
	{
		psendbuf[HEAD_LEN] = 2;
		psendbuf[HEAD_LEN+1] = 0x01;
		psendbuf[HEAD_LEN+2] = 0xFD;
		int len = HEAD_LEN + 1 + psendbuf[HEAD_LEN];
		p->write2(psendbuf, len);
	}

	system("reboot");
	
	return true;
}






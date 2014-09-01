

#ifndef _IPRINTER_H_
#define _IPRINTER_H_

#include "base/sy_mutex.h"
#include "sy_thread.h"
#include "Manager/sy_configManager.h"
#include "NetService/sy_netService.h"

#define MAX_LEN (1024*2)
class IPrinter : public CThread
{
public:
	int getTypte();
	bool setType(int type);//0-并口，1-串口，2-网络
	int getSpeed();
	bool setSpeed(int speed);
	bool setIP(char* ip);
	IPrinter(int type, CConfigTable& tb,IPrinter* pPrinter=NULL);//0-输入 1-输出 
	bool put(const char* dat,int len);//向缓冲区写入数据
	bool putfile(const char* file);//把文件内容写入并口缓冲区
	int left(void);//剩余缓冲区大小
	int showbuf(char* dat, int len);//显示缓冲区内容，dat由调用者申请，并提供最大长度，返回实际长度
	
	bool autowelcome;
	char cip[32];

private:
	
	void ThreadProc();
	char cbuf[MAX_LEN];
	int rd,wr;
	bool fullflag,loopflag;
	CMutex	m_mutex;
	int ctype,cspeed;
	int mtype;//0-输入 1-输出

	int fd;
	INetService* pWriter;//给输入端口传毒输出指针

};

#endif

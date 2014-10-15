/*
 * main.cpp
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */

#include "base/sy_types.h"
#include "base/sy_semaphore.h"
#include "base/sy_debug.h"
#include "Manager/sy_configManager.h"
#include "NetService/sy_netService.h"
#include "rpc/sy_netrpc.h"
#include "user/sy_user.h"
#include "sy_printer.h"
#include "Device/sy_device.h"
#include "sy_upgrade.h"


const char * mainPath = "/config/Config";
const char * defaultPath = "/tmp/Default";
const char *PPP = "/dev/pp";

IPrinter *gPrintOut=NULL;
//IPrinter gPrintOut;
IUpgrade *gUpgrade;


int PP_fd;

void rewrite_software();
int main(int argc, char* argv[])
{
	infof("app start!\n");
	infof("mainPath = [%s]\n", mainPath);
	infof("defaultPath = [%s]\n", defaultPath);
	signal(SIGPIPE, SIG_IGN);

	INetRpc::instance()->start();
	PP_fd = open(PPP, O_RDWR);
	if(PP_fd <=0)
		return -1;

	
	CConfigTable aConfig;
	IConfigManager::config(mainPath, defaultPath);
	IConfigManager::instance()->getConfig("All", aConfig);
	IDevice::instance()->setConfig1("tcp-ip", aConfig["tcp-ip"]);
	IConfigManager::instance()->setConfig("tcp-ip", aConfig["tcp-ip"]);//用于把实际mac地址回写入配置
	rewrite_software();
	IUser::instance();
	//ILog::instance();
		
	gUpgrade = new IUpgrade(); 

	//程序起来后需要响一下
	IDevice::instance()->setLed(IDevice::LED_ALARM, 1);
	sleep(1);
	IDevice::instance()->setLed(IDevice::LED_ALARM, 0);

	IDevice::instance()->setLed(IDevice::LED_BOOT, 2);
	while(0)
	{
		sleep(1);
	}

	IPrinter printOut = IPrinter(1, aConfig["PrinterOut"]);
	gPrintOut = &printOut;


	INetService::instance()->setConfig("NetService", aConfig["NetService"]);
	tracepoint();
	ICtlNetService::instance()->setConfig("NetService", aConfig["NetService"]);
	tracepoint();

	IPrinter printIn = IPrinter(0, aConfig["PrinterIn"], &printOut);//a=打印机输入，b=打印机输出
	
	
	//infof("<%p>\n", INetService::instance());

	CSemaphore sem;
	sem.Pend();

	return 0;
}
void rewrite_software()
{
	FILE* fp;
	if((fp = popen("sh /app/bin/ver", "r")) == NULL)
		return;

	char buf[128]="";
	fgets(buf, sizeof(buf), fp);
	pclose(fp);
	for(int i=0;i<128;i++)
	{
		if(buf[i]=='\n')
			buf[i]='\0';
	}
	infof("ver is [%s]\n", buf);
	CConfigTable aConfig;
	IConfigManager::instance()->getConfig("All", aConfig);
	std::string ver = buf;
	aConfig["base"]["sw"] = ver;
	IConfigManager::instance()->setConfig("base", aConfig["base"]);
}

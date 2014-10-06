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

IPrinter *gPrintOut=NULL;
//IPrinter gPrintOut;
IUpgrade *gUpgrade;


int main(int argc, char* argv[])
{
	infof("app start!\n");
	CConfigTable aConfig;
	IConfigManager::config(mainPath, defaultPath);
	IConfigManager::instance()->getConfig("All", aConfig);
	IDevice::instance()->setConfig1("tcp-ip", aConfig["tcp-ip"]);
	IConfigManager::instance()->setConfig("tcp-ip", aConfig["tcp-ip"]);//用于把实际mac地址回写入配置
	IUser::instance();
	//ILog::instance();
	INetRpc::instance()->start();
		
	gUpgrade = new IUpgrade(); 
	IDevice::instance()->setLed(IDevice::LED_BOOT, 1);
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

/*
 * sy_device.cpp
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#include "base/sy_debug.h"
#include "base/sy_semaphore.h"
#include "Manager/sy_file.h"
#include "Manager/sy_configManager.h"
#include "base/sy_guard.h"
#include "sy_printer.h"
#include "sy_device.h"
#include "sy_printer.h"

#include "NetService/sy_netService.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "net/if.h"
#include "arpa/inet.h"
#include "linux/sockios.h"


const char PP[] = "/dev/pp";


const char* dev="eth0";

//ioctl
#define PP_CMD_MAGIC	'P'
#define PP_IOCTL_LED_1_CTL		_IOW(PP_CMD_MAGIC,0,int)
#define PP_IOCTL_LED_2_CTL		_IOW(PP_CMD_MAGIC,1,int)
#define PP_IOCTL_LED_3_CTL		_IOW(PP_CMD_MAGIC,2,int)
#define PP_IOCTL_LED_4_CTL		_IOW(PP_CMD_MAGIC,3,int)
#define PP_IOCTL_LED_5_CTL		_IOW(PP_CMD_MAGIC,4,int)
#define PP_IOCTL_LED_6_CTL		_IOW(PP_CMD_MAGIC,5,int)
#define PP_IOCTL_BUZZ_CTL		_IOW(PP_CMD_MAGIC,6,int)
#define PP_IOCTL_IP175C_CTL		_IOW(PP_CMD_MAGIC,8,int)
#define PP_IOCTL_GET_POS_STATUS	_IOW(PP_CMD_MAGIC,9,int)
#define PP_IOCTL_SET_PC_STATUS	_IOW(PP_CMD_MAGIC,10,int)
#define PP_IOCTL_GET_TX_STATUS	_IOW(PP_CMD_MAGIC,12,int)
#define PP_IOCTL_GET_RX_STATUS	_IOW(PP_CMD_MAGIC,13,int)

PATTERN_SINGLETON_IMPLEMENT(CDevice)

CDevice::~CDevice()
{
	close(fd);
}

CDevice::CDevice()
{
	strncpy(this->str, "tcp-ip", sizeof(str));
	IConfigManager::instance()->reg(this);

	this->fd = open(PP, O_RDWR);
	if(this->fd <= 0)
	{
		errorf("open pp failed\n");
	}
	infof("fd = %d\n", this->fd);

	int ctl_data = 0;//关闭所有指示灯
	//ioctl(fd, PP_IOCTL_LED_1_CTL, &ctl_data);
	ioctl(fd, PP_IOCTL_LED_2_CTL, &ctl_data);//2
	ioctl(fd, PP_IOCTL_LED_3_CTL, &ctl_data);//3
	ioctl(fd, PP_IOCTL_LED_4_CTL, &ctl_data);//4
	ioctl(fd, PP_IOCTL_LED_5_CTL, &ctl_data);//5绿色
	ioctl(fd, PP_IOCTL_LED_6_CTL, &ctl_data);//5红色
	ioctl(fd, PP_IOCTL_BUZZ_CTL, &ctl_data);
	//while(1);
	for(int i = 0 ; i < MAX_LED_NUM; i++)
	{
		msetLed[i] = 0;
		msetLedStat[i] = 0;
	}

	this->CreateThread();
}

void CDevice::ThreadProc()
{
	int i;
	while(1)
	{
		for(i=0;i<MAX_LED_NUM;i++)
		{
			switch(msetLed[i])
			{
				case 0://灭
				case 1://亮
					in_setLed((LED_NAME)i, msetLed[i]);
					break;
				case 2://闪烁
					in_setLed((LED_NAME)i, msetLedStat[i]);
					//infof("msetLedStat[%d] = %d\n", i, msetLedStat[i]);
					msetLedStat[i]=(msetLedStat[i]+1) & 0x01;
					break;
				default:break;
			}
		}
		usleep(500000);
	}
}

int CDevice::get_tx_status(int* p)
{

	ioctl(fd, PP_IOCTL_GET_TX_STATUS, &buf_status);
	p = buf_status.buf;
	return buf_status.buf_length;

}
bool CDevice::setLed(LED_NAME nm, int state)
{
	msetLed[nm] = state;
	infof("msetLed[%d] = %d\n", nm, msetLed[nm]);
	return true;
}

bool CDevice::in_setLed(LED_NAME nm, int state)
{
	int ctl_data;
	switch(nm)
	{
		case LED_ALARM://１报警双色，２启动，３连接，４升级，５给驱动用
			ioctl(fd, PP_IOCTL_LED_5_CTL, &state);
			ioctl(fd, PP_IOCTL_LED_6_CTL, &state);
			ioctl(fd, PP_IOCTL_BUZZ_CTL, &state);
			//infof("state = %d\n", state);
			break;
		case LED_BOOT:
			ioctl(fd, PP_IOCTL_LED_4_CTL, &state);
			break;
		case LED_CONN:
			ioctl(fd, PP_IOCTL_LED_3_CTL, &state);
		case LED_UPDATE:
			ioctl(fd, PP_IOCTL_LED_2_CTL, &state);
			break;
		default:break;

	}
	return true;
}

bool CDevice::setConfig1(const char* name, CConfigTable& table)
{
	if(strcmp(name, "tcp-ip") == 0)
	{
		char hd[6];
		getMAC(hd);
		char tmp[32]="";
		sprintf(tmp, "%02X:%02X:%02X:%02X:%02X:%02X", hd[0], hd[1], hd[2], hd[3], hd[4], hd[5]);
		table["mac"] = CConfigTable(tmp);
		return this->setNetWork(table);
	}

	return false;
}

bool CDevice::setConfig(const char* name, const CConfigTable& table)
{
	if(strcmp(name, "tcp-ip") == 0)
	{
		return this->setNetWork(table);
	}

	return false;
}

bool CDevice::setNetWork(const CConfigTable& table)
{

	std::cout << "network: " << table << std::endl;

	std::string mode = table["mode"].asCString();
	if(mode == "static")
	{
		tracepoint();
		
		if(this->setsth("ip", table["ip"]) == false)
			return false;
		if(this->setsth("netmask", table["netmask"]) == false)
			return false;
		if(this->setgateway(table["gateway"]) == false)
			return false;
		INetService::instance()->restart();
		ICtlNetService::instance()->restart();
		tracepoint();
		//infof("-------------------------\n");
		//sleep(3);
		//reboot(LINUX_REBOOT_CMD_RESTART);

		//system("reboot");
		return true;
	}
	else
	{
		return false;
	}
}

bool CDevice::setsth(const char* name, const CConfigTable& table)
{
	int ret;
	struct ifreq ifr;

	strcpy(ifr.ifr_name, dev);
	
	int type,set;



	if(strcmp(name, "ip") == 0)
	{
		type = SIOCGIFADDR;
		set = SIOCSIFADDR;

	}
	else if(strcmp(name, "netmask") == 0)
	{
		type = SIOCGIFNETMASK;
		set = SIOCSIFNETMASK;
	}
	else
	{
		errorf("unknown type is [%s]\n!", name); 
		return false;
	}

	int fd = socket(AF_INET,SOCK_DGRAM,0);

	//get inet addr
    if( ioctl( fd, type, &ifr) == -1)
    {
    	errorf("ioctl error!\n");
    	return false;
    }


    struct sockaddr_in *addr = (struct sockaddr_in *)&(ifr.ifr_addr);
    char * address = inet_ntoa(addr->sin_addr);

    //set inet addr
    struct sockaddr_in *p = (struct sockaddr_in *)&(ifr.ifr_addr);

    p->sin_family = AF_INET;

    std::string cst;
    cst = table.asCString();
    inet_aton( cst.c_str(), &(p->sin_addr));

    if( ioctl( fd, set, &ifr) == -1)
    {
    	errorf("ioctl error.\n");
    	ret = false;
    }
    else    
    {
    	infof("change inet addr to: %s\n", cst.c_str());
    	ret = true;
    }

    close(fd);
    return ret;

}


bool CDevice::setgateway(const CConfigTable& table)    
{    
	int skfd;    
	struct rtentry rt;    
	int err;    

	skfd = socket(PF_INET, SOCK_DGRAM, 0);    
	if (skfd < 0)    
		return false;    

	struct sockaddr_in gw;
	std::string cst;
	cst = table.asCString();
	inet_aton( cst.c_str(), &(gw.sin_addr));


	/* Delete existing defalt gateway */    
	memset(&rt, 0, sizeof(rt));    

	rt.rt_dst.sa_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_dst)->sin_addr.s_addr = 0;    

	rt.rt_genmask.sa_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_genmask)->sin_addr.s_addr = 0;    

	rt.rt_flags = RTF_UP;    

	err = ioctl(skfd, SIOCDELRT, &rt);    

	if ((err == 0 || errno == ESRCH) && gw.sin_addr.s_addr > 0) {    
	/* Set default gateway */    
	memset(&rt, 0, sizeof(rt));    

	rt.rt_dst.sa_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_dst)->sin_addr.s_addr = 0;    

	rt.rt_gateway.sa_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_gateway)->sin_addr.s_addr = gw.sin_addr.s_addr;    

	rt.rt_genmask.sa_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_genmask)->sin_addr.s_addr = 0;    

	rt.rt_flags = RTF_UP | RTF_GATEWAY;    

	err = ioctl(skfd, SIOCADDRT, &rt);    
	}    

	close(skfd);    

	return true;    
}    

bool CDevice::getMAC(char* hd)  
{
	int skfd;    
	struct ifreq ifr;

	strcpy(ifr.ifr_name, dev);

	skfd = socket(PF_INET, SOCK_DGRAM, 0);    
	if (skfd < 0)    
		return false;    

	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) == -1)
	{
        errorf("hwaddr error.\n");
    	return false;
    }

	memcpy( hd, ifr.ifr_hwaddr.sa_data, 6);

	infof("HWaddr: %02X:%02X:%02X:%02X:%02X:%02X\n", hd[0], hd[1], hd[2], hd[3], hd[4], hd[5]);

	close(skfd);    


	return true;

}

IDevice *IDevice::instance()
{
	return CDevice::instance();
}

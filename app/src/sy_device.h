/*
 * sy_device.h
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#ifndef _SY_DEVICE_H_
#define _SY_DEVICE_H_

#include "json/value.h"
#include "json/writer.h"
#include "json/reader.h"

#include "base/sy_types.h"
#include "base/sy_mutex.h"
#include "base/sy_singletion.h"
#include "base/sy_net.h"
#include "Device/sy_device.h"
#include "Manager/sy_configManager.h"
#include "sy_thread.h"


class CDevice : public IDevice, public ISetConfig
{
public:
	PATTERN_SINGLETON_DECLARE(CDevice)

	bool setConfig(const char* name,const CConfigTable& table);//设置后自动生效
	bool setConfig1(const char* name, CConfigTable& table);
	bool getMAC(char* hd);
	bool setLed(LED_NAME nm, int state);

private:
	bool setNetWork(const CConfigTable& table);
	bool setsth(const char* name, const CConfigTable& table);
	bool setgateway(const CConfigTable& table); 

	int fd;   

};

#endif

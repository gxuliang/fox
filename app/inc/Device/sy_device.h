/*
 * sy_device.h
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#ifndef _EX_SY_DEVICE_H_
#define _EX_SY_DEVICE_H_


class IDevice
{
public:
	enum LED_NAME
	{  
		LED_BOOT = 1,
		LED_CONN	,
		LED_ALARM	,
	};
	static IDevice *instance();
	virtual ~IDevice(){};
	virtual bool setConfig(const char* name, const CConfigTable& table) = 0;
	virtual bool setConfig1(const char* name, CConfigTable& table) = 0;

	virtual bool getMAC( char* hd) = 0;

	virtual bool setLed(LED_NAME nm, int state) = 0;

};



 #endif
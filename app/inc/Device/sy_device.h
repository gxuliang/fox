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
	{  //１报警双色，２启动，３连接，４升级，５给驱动用
		LED_ALARM = 0,
		LED_BOOT	,
		LED_CONN	,
		LED_UPDATE	,
	};
	static IDevice *instance();
	virtual ~IDevice(){};
	virtual bool setConfig(const char* name, const CConfigTable& table) = 0;
	virtual bool setConfig1(const char* name, CConfigTable& table) = 0;

	virtual bool getMAC( char* hd) = 0;

	virtual bool setLed(LED_NAME nm, int state) = 0;

	virtual 	int get_tx_status(int* p) =0;

};



 #endif
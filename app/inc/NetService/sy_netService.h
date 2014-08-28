/*
 * sy_netService.h
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#ifndef _EX_SY_NETSERVICE_H_
#define _EX_SY_NETSERVICE_H_

class INetService
{
public:
	static INetService *instance();
	virtual ~INetService(){};
	virtual bool setConfig(const char* name, const CConfigTable& table) = 0;//设置后自动生效
	virtual bool getState(CConfigTable& table) = 0;//获取连接状态
};



 #endif

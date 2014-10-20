/*
 * sy_netServ.h
 *
 *  Created on: 2014-10-20
 *      Author: xul
 */

#ifndef _EX_SY_NETSERV_H_
#define _EX_SY_NETSERV_H_


class INetServ
{
public:
	static INetServ *instance();
	virtual ~INetServ(){};
	virtual bool setConfig(const char* name, const CConfigTable& table) = 0;//设置后自动生效
	//virtual bool getState(CConfigTable& table) = 0;//获取连接状态
	virtual int write(const char*, int) = 0;
	virtual int write2(const char*, int) = 0;
	virtual	bool restart()=0;
	virtual void conn_clear()=0;
	virtual bool getState()=0;


};


#endif

/*
 * sy_net.h
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#ifndef _SY_NET_H_
#define _SY_NET_H_
 
#include "base/sy_mutex.h"


class MySock
{
public:
	virtual ~MySock(){};
	virtual int read(void* , int , int tm = 0) = 0;
	virtual int write(const void* , int ) = 0;
	virtual void perror(const char* info) = 0;
	virtual	bool close() = 0;
};

class MyClient : public MySock
{
public:
	enum SockType
	{
		mTCP = 0x01,
		mUDP = 0x02,
	};

	MyClient();
	~MyClient();
	bool connect(const char* ip, const ushort port, const int tm=0);
	int read(void* , int , int tm = 0);
	int write(const void* , int );
	bool close();
	void perror(const char* info);
	bool conn_flag;
	int fd;

private:
	struct sockaddr_in addr;
	socklen_t addrlen;
	CMutex		m_mutex;

};


#endif

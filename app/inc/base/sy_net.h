/*
 * sy_net.h
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#ifndef _SY_NET_H_
#define _SY_NET_H_


class MySock
{
public:
	virtual ~MySock(){};
	virtual int read(void* , int ) = 0;
	virtual int write(void* , int ) = 0;
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
	bool connect(const char* ip, const ushort port);
	int read(void* , int );
	int write(void* , int );
	bool close();
	void perror(const char* info);
	bool conn_flag;
};


#endif

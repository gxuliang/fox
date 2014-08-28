/*
 * sy_net.cpp
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#include "base/sy_types.h"
#include "base/sy_net.h"

MyClient::MyClient()
{
	fd = 0;
	conn_flag = false;
}

MyClient::~MyClient()
{
	close();
}

int MyClient::read(void* buf, int len)
{
	int ret;

	ret = ::read(fd, buf, len);
	return ret;
}

	
int MyClient::write(void* buf, int len)
{
	int ret;

	ret = ::write(fd, buf, len);
	return ret;
}

bool MyClient::close()
{
	bool ret = false;

	if(::close(fd) == 0)
		ret = true;

	conn_flag = false;
	return ret;
}

bool MyClient::connect(const char* ip, const ushort port)
{

	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		perror("socket");
		return false;
	}
	::bzero(&addr, sizeof(struct sockaddr_in));
	addrlen = sizeof(struct sockaddr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	if(::connect(fd, (struct sockaddr*)&addr, addrlen) < 0)
	{
		perror("connect");
		close();
		return false;
	}

	conn_flag = true;
	return true;
}
	
void MyClient::perror(const char* info)
{
	std::cout << "111" << std::endl;
	::perror(info);
}
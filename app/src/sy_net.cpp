/*
 * sy_net.cpp
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#include "base/sy_types.h"
#include "base/sy_debug.h"
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

int MyClient::read(void* buf, int len, int timeout)
{
	int ret;
	fd_set readfds;

	if(fd <=0 || conn_flag == false)
		return false;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	struct timeval overtime;
	overtime.tv_sec = 0;
	overtime.tv_usec = 1000*timeout;
	if(timeout == 0)
	{
		ret = ::select(fd+1, &readfds, NULL, NULL, NULL);
	}
	else
		ret = ::select(fd+1, &readfds, NULL, NULL, &overtime);
	if(ret <= 0)
		return ret;
	else
		ret = ::recv(fd, buf, len, 0);
	return ret;
}

	
int MyClient::write(const void* buf, int len)
{
	int ret;
	warnf("fd = %d, buf is %p, len = %d\n", fd, buf, len);
	if(fd <=0 || conn_flag == false)
		return false;

	ret = ::send(fd, buf, len,0);
	if(ret < 0)
		perror("::send");

	return ret;
}

bool MyClient::close()
{
	bool ret = false;
	std::cout << "111:" << fd << std::endl;
	
	if(::close(fd) == 0)
	{
		std::cout << "222" << std::endl;
		ret = true;
	}
	
	std::cout << "333" << std::endl;
	conn_flag = false;
	return true;
}

bool MyClient::connect(const char* ip, const ushort port)
{
	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	debugf("========================fd = %d\n", fd);
	if(fd <= 0)
	{
		perror("socket");
		return false;
	}
	unsigned long ul = 1;
	ioctl(fd, FIONBIO, &ul); //设置为非阻塞模式

	::bzero(&addr, sizeof(struct sockaddr_in));
	addrlen = sizeof(struct sockaddr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	if(::connect(fd, (struct sockaddr*)&addr, addrlen) < 0)
	{
		struct timeval tm;
		fd_set set;
		int error, len;

		tm.tv_sec  = 5;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        if( ::select(fd+1, NULL, &set, NULL, &tm) > 0)
        {
        	getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
        	if(error == 0)
        	{	
        		conn_flag = true;
        		return true;
        	}
        }

		perror("connect");
		close();
		return false;
	}

	conn_flag = true;
	return true;
}
	
void MyClient::perror(const char* info)
{
	//std::cout << "111" << std::endl;
	::perror(info);
}
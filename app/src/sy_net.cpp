/*
 * sy_net.cpp
 *
 *  Created on: 2014-8-27
 *      Author: xul
 */

#include "base/sy_types.h"
#include "base/sy_debug.h"
#include "base/sy_net.h"
#include "base/sy_guard.h"


MyClient::MyClient(): m_mutex(CMutex::mutexRecursive)
{
	fd = -1;
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
	{
		tracepoint();
		return false;
	}
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
	{
		ret = ::select(fd+1, &readfds, NULL, NULL, &overtime);
	}

	if(ret <= 0)
	{
		tracepoint();
		return ret;
	}
	else
	{
		tracepoint();
		ret = ::recv(fd, buf, len, 0);
		if(ret <0)
			printf("recv error =%d\n", errno);
		printf("ret =%d\n", ret);
	}
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
	CGuard guard(m_mutex);

	bool ret = false;
	//std::cout << "111:" << fd << std::endl;
	
	debugf("it will close fd = %d\n", fd);

	if(::close(fd) == 0)
	{
		tracepoint();
		ret = true;
		fd = -1;
	}
	
	//std::cout << "333" << std::endl;
	conn_flag = false;
	return true;
}

bool MyClient::connect(const char* ip, const ushort port, const int timeout)
{
	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	int ret;
	debugf("========================fd = %d\n", fd);
	if(fd <= 0)
	{
		perror("socket");
		return false;
	}

	CGuard guard(m_mutex);


	int sendlen = 512*1024;
	ret = ::setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sendlen, sizeof(int));
	debugf("========================ret = %d\n", ret);

	ret = ::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sendlen, sizeof(int));
	debugf("========================ret = %d\n", ret);


	unsigned long ul = 1;
	int flags;

	if(timeout > 0)
	{
		//ioctl(fd, FIONBIO, &ul); //设置为非阻塞模式
		flags = fcntl(fd, F_GETFL, 0);  
    	fcntl(fd, F_SETFL, flags | O_NONBLOCK);  
	}

	::bzero(&addr, sizeof(struct sockaddr_in));
	addrlen = sizeof(struct sockaddr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	tracepoint();
	if(::connect(fd, (struct sockaddr*)&addr, addrlen) < 0)
	{
		if (errno != EINPROGRESS)
		{  
			tracepoint();
			perror("connect");
			close();
			sleep(1);
        	return false; 
    	}
		if(timeout > 0)
		{
			struct timeval tm;
			fd_set set;
			int error, len;

			tm.tv_sec  = timeout;
	        tm.tv_usec = 0;
	        FD_ZERO(&set);
	        FD_SET(fd, &set);
	        debugf("timeout = %d\n", timeout);
	        if( (ret = ::select(fd+1, NULL, &set, NULL, &tm)) > 0)
	        {
	        	getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
	        	if(error == 0)
	        	{	
	        		conn_flag = true;
	        		ul = 0;
	        		//ioctl(fd, FIONBIO, &ul); //设置回阻塞模式
	        		fcntl(fd, F_SETFL, flags);  /* restore file status flags */ 
	        		return true;
	        	}
	        	else
	        	{
	        		debugf("error = %d, ret = %d\n", error, ret);
	        		usleep(500*1000);
	        	}
	        }
    	}
		
		tracepoint();
		perror("connect");
		close();
		return false;
	}
	tracepoint();
	conn_flag = true;
	return true;
}
	
void MyClient::perror(const char* info)
{
	//std::cout << "111" << std::endl;
	::perror(info);
}
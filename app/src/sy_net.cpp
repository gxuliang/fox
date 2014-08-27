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

}

MyClient::~MyClient()
{
	close();
}

int MyClient::read(void* buf, int len)
{
	int ret;

	return ret;
}

	
int MyClient::write(void* buf, int len)
{
	int ret;

	return ret;
}

bool MyClient::close()
{
	bool ret;

	return ret;
}

bool MyClient::connect(const char* ip, const ushort port)
{
	bool ret;

	return ret;
}
	
void MyClient::perror(const char* info)
{
	//std::perror(info);
}
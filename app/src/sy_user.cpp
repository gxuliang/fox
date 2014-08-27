/*
 * sy_user.cpp
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#include "base/sy_debug.h"
#include "sy_user.h"
#include "Manager/sy_configManager.h"


PATTERN_SINGLETON_IMPLEMENT(CUser)


CUser::~CUser()
{

}


CUser::CUser() : m_mutex(CMutex::mutexRecursive)
{


}


bool CUser::rpclogin(const CConfigTable& root, CConfigTable& response)
{
	debugf("%s", root["params"]["userName"].asCString());
	CConfigTable a;
	IConfigManager::instance()->getConfig("user", a);
	for(uint i = 0; i < a.size(); i++)
	{
		if(root["params"]["userName"].asString() == a[i]["name"].asString() && 
			root["params"]["password"].asString() == a[i]["password"].asString())
		{
			response["result"]=true;
			response["session"] = 123456;
			return true;	
		}
	}
	response["result"]=false;
	return false;
}
bool CUser::rpclogout(const CConfigTable& root, CConfigTable& response)
{

}
bool CUser::rpckeepalive(const CConfigTable& root, CConfigTable& response)
{

}

bool CUser::showusr()
{

}

void CUser::onTimer()
{

}

bool IUser::rpclogin(const CConfigTable& root, CConfigTable& response)
{
	return CUser::rpclogin(root, response);
}

bool IUser::rpclogout(const CConfigTable& root, CConfigTable& response)
{
	return CUser::rpclogout(root, response);
}

bool IUser::rpckeepalive(const CConfigTable& root, CConfigTable& response)
{
	return CUser::rpckeepalive(root, response);
}


IUser *IUser::instance()
{
	return CUser::instance();
}

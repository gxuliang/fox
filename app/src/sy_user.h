/*
 * sy_user.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef SY_USER_H_
#define SY_USER_H_

#include "user/sy_user.h"
#include "base/sy_singletion.h"
#include "base/sy_mutex.h"

struct Session
{
	uint id;
	char name[32];
	char client[64];
	char ipaddr[64];
};
struct Usr
{
	Session ses;
	Usr* next;
};
class CUser : public IUser
{
	PATTERN_SINGLETON_DECLARE(CUser)
public:
	static bool rpclogin(const CConfigTable& root, CConfigTable& response);
	static bool rpclogout(const CConfigTable& root, CConfigTable& response);
	static bool rpckeepalive(const CConfigTable& root, CConfigTable& response);
	bool showusr();
private:
	void onTimer();
private:
	Usr m_usr;
	CMutex	m_mutex;
};



#endif /* SY_USER_H_ */

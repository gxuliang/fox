/*
 * sy_user.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef EX_SY_USER_H_
#define EX_SY_USER_H_

#include <json/json.h>
#include "base/sy_types.h"

class IUser
{
public:
	static bool rpclogin(const CConfigTable& root, CConfigTable& response);
	static bool rpclogout(const CConfigTable& root, CConfigTable& response);
	static bool rpckeepalive(const CConfigTable& root, CConfigTable& response);

	static IUser *instance();

	virtual ~IUser(){};
	virtual bool showusr() = 0;
};


#endif /* SY_USER_H_ */

/*
 * sy_netrpc.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef EX_SY_NETRPC_H_
#define EX_SY_NETRPC_H_

#include "base/sy_types.h"

class INetRpc
{
public:
	static INetRpc *instance();
	virtual bool start(void) = 0;

};


#endif /* SY_NETRPC_H_ */

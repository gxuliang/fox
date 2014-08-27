/*
 * sy_netrpc.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef SY_NETRPC_H_
#define SY_NETRPC_H_

#include "rpc/sy_netrpc.h"
#include "base/sy_singletion.h"

class CNetRpc : public INetRpc
{
	PATTERN_SINGLETON_DECLARE(CNetRpc)
public:
	bool start(void);

private:
	static void* proc(void* fun);
private:
	bool m_run;
	Json::Rpc::TcpServer server;

};


#endif /* SY_NETRPC_H_ */

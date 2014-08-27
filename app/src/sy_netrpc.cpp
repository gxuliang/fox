/*
 * sy_netrpc.cpp
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#include "base/sy_types.h"
#include "base/sy_debug.h"


#include "jsonrpc/jsonrpc.h"
#include "sy_netrpc.h"
#include "sy_rpcmethod.h"

PATTERN_SINGLETON_IMPLEMENT(CNetRpc)

static SYRpc m_a;
CNetRpc::~CNetRpc()
{

}

CNetRpc::CNetRpc() : server(std::string("127.0.0.1"), 8086)
{
	m_run = false;



	if(!networking::init())
	{
		errorf("Networking initialization failed");
		exit(EXIT_FAILURE);
	}

	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::Print, std::string("print")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::Notify, std::string("notify")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::GetRoot, std::string("getroot")));
	//method
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::Glogin, std::string("global.login")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::Glogout, std::string("global.logout")));
	//confgig
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::CgetConfig, std::string("configManager.getConfig")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::CsetConfig, std::string("configManager.setConfig")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::CgetDefault, std::string("configManager.getDefault")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::CsetDefault, std::string("configManager.setDefault")));
	//media
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::MgetMedia, std::string("Media.getMedia")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::MstopMedia, std::string("Media.stopMedia")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::MCtrlMedia, std::string("Media.ctrlMedia")));

	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::getState, std::string("State.getState")));
	server.AddMethod(new Json::Rpc::RpcMethod<SYRpc>(m_a, &SYRpc::SendCmd, std::string("State.SendCmd")));

	if(!server.Bind())
	{
		errorf("Bind failed");
		exit(EXIT_FAILURE);
	}

	if(!server.Listen())
	{
		errorf("Listen failed");
		exit(EXIT_FAILURE);
	}
}

INetRpc *INetRpc::instance()
{
	return CNetRpc::instance();
}

bool CNetRpc::start(void)
{
	m_run = true;

	pthread_t ntid;
	bool ret = pthread_create(&ntid, NULL, proc, (void*)this);
	if(ret)
	{
		return pthread_detach(ntid);
	}
	return ret;
}
void* CNetRpc::proc(void* fun)
{
	CNetRpc *p = (CNetRpc*)fun;
	infof("Start JSON-RPC TCP server");
	while(p->m_run)
	{
		//tracepoint();
		p->server.WaitMessage(1000);
	}
}

/*
 * sy_rpcmthod.h
 *
 *  Created on: 2013-3-21
 *      Author: xul
 */

#ifndef SY_RPCMTHOD_H_
#define SY_RPCMTHOD_H_


#include <json/json.h>

/**
 * \class TestRpc
 * \brief RPC example.
 */
class SYRpc
{
  public:
    /**
     * \brief Reply with success.
     * \param root JSON-RPC request
     * \param response JSON-RPC response
     * \return true if correctly processed, false otherwise
     */
    bool Print(const Json::Value& root, Json::Value& response);

    /**
     * \brief Notification.
     * \param root JSON-RPC request
     * \param response JSON-RPC response
     * \return true if correctly processed, false otherwise
     */
    bool Notify(const Json::Value& root, Json::Value& response);

    bool GetRoot(const Json::Value& root, Json::Value& response);

    /**
     * \brief Get the description in JSON format.
     * \return JSON description
     */
    Json::Value GetDescription();

    bool Glogin(const Json::Value& root, Json::Value& response);
    bool Glogout(const Json::Value& root, Json::Value& response);

    bool CgetConfig(const Json::Value& root, Json::Value& response);
    bool CsetConfig(const Json::Value& root, Json::Value& response);
    bool CgetDefault(const Json::Value& root, Json::Value& response);
    bool CsetDefault(const Json::Value& root, Json::Value& response);
    bool MgetMedia(const Json::Value& root, Json::Value& response);
    bool MstopMedia(const Json::Value& root, Json::Value& response);
    bool MCtrlMedia(const Json::Value& root, Json::Value& response);
    bool getState(const Json::Value& root, Json::Value& response);
    bool SendCmd(const Json::Value& root, Json::Value& response);

};

#endif /* SY_RPCMTHOD_H_ */

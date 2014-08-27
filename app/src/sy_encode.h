/*
 * sy_encode.h
 *
 *  Created on: 2013-4-15
 *      Author: xul
 */

#ifndef SY_ENCODE_H_
#define SY_ENCODE_H_

#include "base/sy_types.h"
#include "inc_comm/com_encode.h"
#include "base/sy_mutex.h"

#define	CHANNEL	0
#define	MAX_FRAME	3//主码流、子码流、抓拍码流

enum
{
	CovPX = 0x01,//转换分辨率
	CovComp,//编码模式
	ConStrm,//码流控制
};
class CEncode
{
public:
	CEncode();
	static bool config(const char* wpath, const char* rpath);
	bool setEncode(const CConfigTable &table, const uint channel);
	static bool setCamGloble(const CConfigTable &table);
private:
	static int w_fd, r_fd;
	uint mtype;//标识主码流、辅码流、抓图码流
	static CMutex	m_mutex;
	EncodeCom m_frame[CHANNEL];
	static CamGlobal mcamglo;
};


#endif /* SY_ENCODE_H_ */

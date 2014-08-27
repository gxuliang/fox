/*
 * sy_media.h
 *
 *  Created on: 2013-8-14
 *      Author: xul
 */

#ifndef _EX_SY_MEDIA_H_
#define _EX_SY_MEDIA_H_


class IMedia
{
public:
	static void config(const char* mainPath);//读取能力集配置
	virtual bool getConfig()=0;
	virtual bool setConfig(CConfigTable &cfg, int &errno)=0;//err可能是能力不匹配、参数错误等
	virtual bool setOSD()=0;//参数是个结构体，内容包含字符、坐标、颜色等，应该是数组,暂时按每个通道都可以独立设置OSD来
	//
};

#endif /* SY_MEDIA_H_ */

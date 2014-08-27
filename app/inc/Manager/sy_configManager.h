/*
 * sy_configManager.h
 *
 *  Created on: 2013-3-19
 *      Author: xul
 */

#ifndef _EX_SY_CONFIGMANAGER_H_
#define _EX_SY_CONFIGMANAGER_H_

class ISetConfig
{
public:
	virtual ~ISetConfig(){};
	char str[32];
	virtual bool setConfig(const char* name, const CConfigTable& table){};
};

class IConfigManager
{
public:

	/// 配置应用方式或者结果，按位表示，由配置的观察者函数和commit调用前后都可以
	/// 设置和检查
	enum ApplyOptions
	{
		applySuccess			= 0x00000000,	///< 成功
		applyFailed				= 0x0000006c,	///< 失败掩码
		applyNeedRestart		= 0x00000001,	///< 需要重启应用程序
		applyNeedReboot			= 0x00000002,	///< 需要重启系统
		applyWriteFileError		= 0x00000004,	///< 写文件出错
		applyCapsNotSupport		= 0x00000008,	///< 特性不支持
		applyValidateFailed		= 0x00000020,	///< 验证失败
		applyNotExist			= 0x00000040,	///< 配置不存在，根据配置名访问是可能会发生
		applyDelaySave			= 0x00000100,	///< 延时保存，commit函数传入
		applyWithoutLog			= 0x00000200,	///< 提交是不要记录日志，commit函数传入
	};


	static void config(const char* mainPath, const char* defaultPath);

	static IConfigManager *instance();

	virtual ~IConfigManager(){};

	virtual bool getDefault(const char* name, CConfigTable& table) = 0;

	virtual void saveFile() = 0;

	// 得到最新的配置结构
	/// \param name 配置名称
	virtual bool getConfig(const char* name, CConfigTable& table) = 0;

	/// 提交配置，含合法性检查，调用saveFile函数保存文件。
	/// \param name 配置名称;当配置名称传"All"的时候保存的是所有配置
	/// \param table 新的配置
	/// \param user 用户名
	/// \param applyOptions 返回值初始值，取ApplyOptions位与值。
	/// \return 配置应用的结果，取ApplyOptions位与值，applyOptions作为返回值的初始值。
	///         (!(返回值&applyFailed))的结果可以表示配置是否应用成功。
	virtual int setConfig(const char* name, const CConfigTable& table, const char* user = 0,
		int applyOptions = applyDelaySave) = 0;

	virtual void reg(ISetConfig*) = 0;

};


#endif /* SY_CONFIGMANAGER_H_ */

/*
 * configManager.h
 *
 *  Created on: 2013-3-19
 *      Author: xul
 */

#ifndef CONFIGMANAGER_H_
#define CONFIGMANAGER_H_

#include "json/value.h"
#include "json/writer.h"
#include "json/reader.h"

#include "base/sy_types.h"
#include "base/sy_mutex.h"
#include "base/sy_singletion.h"
#include "Manager/sy_configManager.h"
#include "sy_thread.h"


#ifndef WIN32
typedef Json::StyledWriter CConfigWriter;
#else
typedef Json::FastWriter CConfigWriter;
#endif
typedef Json::Reader CConfigReader;

bool copyFile(const char* from, const char* to);

class CConfigManager : public IConfigManager, public CThread
{
	PATTERN_SINGLETON_DECLARE(CConfigManager)
public:
	static void config(const char* mainPath, const char* defaultPath);

	bool getConfig(const char* name, CConfigTable& table);

	bool getDefault(const char* name, CConfigTable& table);

	int setConfig(const char* name, const CConfigTable& table, const char* user = 0,
			int applyOptions = applyDelaySave);

	void saveFile();

	void reg(ISetConfig*);

private:
	bool readConfig(const char* path, std::string& input);
	void makedefault(void);
	void ThreadProc();


private:
	static std::string m_mainFilePath;
	static std::string m_defaultFilePath;
	CFile		m_fileConfig;
	CMutex		m_mutex;
	CConfigTable m_configAll;
	CConfigTable m_configDefault;
	CConfigTable m_configCustom;
	bool		m_changed;

	std::string m_stream;

	std::vector<ISetConfig*> svec;
	std::vector<ISetConfig*>::iterator iter;
	int fd[2];
};


#endif /* CONFIGMANAGER_H_ */

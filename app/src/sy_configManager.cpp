/*
 * sy_configManager.cpp
 *
 *  Created on: 2013-3-19
 *      Author: xul
 */

#include "base/sy_debug.h"
#include "Manager/sy_file.h"
#include "base/sy_guard.h"
#include "sy_configManager.h"


PATTERN_SINGLETON_IMPLEMENT(CConfigManager)


std::string CConfigManager::m_mainFilePath		= "";
std::string CConfigManager::m_defaultFilePath	= "";

const int configStreamSize = (256 * 1024);

CConfigManager::~CConfigManager()
{

}

CConfigManager::CConfigManager() : m_mutex(CMutex::mutexRecursive)
{

	CConfigReader reader;

	m_changed = false;
	m_stream.reserve(configStreamSize);

	if (!readConfig(m_defaultFilePath.c_str(), m_stream))
	{
		errorf("default config error[%s]!\n", m_defaultFilePath.c_str());
		assert(0);
	}
	else
	{
		if(!reader.parse(m_stream, m_configDefault))
		{
			errorf("read default error!\n");
			assert(0);
		}
	}

	if (readConfig(m_mainFilePath.c_str(), m_stream))
		{
			if(reader.parse(m_stream, m_configAll))
			{
				tracepoint();
				std::cout << m_configAll << std::endl;
			}
			else
			{
				errorf("read config failed,use default!\n");
				makedefault();
			}
		}
		else
		{
			infof("not found config,using default!");
			makedefault();
		}
}


void CConfigManager::makedefault(void)
{
	CConfigReader reader;
	if(!copyFile(m_defaultFilePath.c_str(), m_mainFilePath.c_str()))
	{
		errorf("CConfigManager::CConfigManager() copy config file failed\n");
		assert(0);
	}
	else
	{
		if (!readConfig(m_defaultFilePath.c_str(), m_stream))
		{
			errorf("default config error!\n");
			assert(0);
		}
		else
		{
			if(reader.parse(m_stream, m_configDefault))
			{
				tracepoint();
				m_configAll = m_configDefault;
				std::cout << m_configAll << std::endl;
			}
			else
			{
				errorf("read default error!\n");
				assert(0);
			}
		}
	}
}

bool CConfigManager::readConfig(const char* path, std::string& input)
{
	if (!m_fileConfig.Open(path, CFile::modeRead | CFile::modeNoTruncate))
		return false;

	const int size = 32*1024;
	char* buf = new char[size + 1];

	input = "";

	while (1)
	{
		int nLen = m_fileConfig.Read(buf, size);

		if(nLen <=0 )
			break;
		buf[nLen] = 0;
		input += buf;
	}
	input += '\0';
	m_fileConfig.Close();
	delete []buf;

	return true;
}

bool CConfigManager::getConfig(const char* name, CConfigTable& table)
{
	CGuard guard(m_mutex);
	int ret = applySuccess;

	if (strcmp(name, "All") == 0)
	{
		table = m_configAll;
		tracef("CConfigManager::getConfig get all Configs.\n");
		return true;
	}

	if(m_configDefault[name].type() == Json::nullValue)
	{
		warnf("CConfigManager::getConfig '%s', but default config is not set yet!\n", name);

		// 直接返回，不做默认配置替换和配置校验
		table = m_configAll[name];
		return table.type() != Json::nullValue;
	}

	table = m_configAll[name];
	tracepoint();
	infof("name is [%s]\n", name);
	//std::cout << table << std::endl;

	return table.type() != Json::nullValue;
}


int CConfigManager::setConfig(const char* name, const CConfigTable& table, const char* user /* = 0 */, int applyOptions /* = applyDelaySave */)
{
	int ret = applyOptions;

	if (strcmp(name, "All") == 0)
	{
		tracepoint();
		if (m_configAll == table)
		{
			return ret;
		}

		m_mutex.Enter();
		Json::Value::Members members( table.getMemberNames() );
		m_mutex.Leave();

		for ( Json::Value::Members::iterator it = members.begin();
			it != members.end();
			++it )
		{
			const std::string &name = *it;

			m_mutex.Enter();
			ret |= setConfig(name.c_str(), table[name], user, applyDelaySave | applyWithoutLog);
			m_mutex.Leave();
		}

		m_mutex.Enter();
		m_changed = true;
		saveFile();
		m_mutex.Leave();

		for(iter = svec.begin(); iter != svec.end(); iter++)
		{
			if(strcmp((*iter)->str, name) == 0)
			{
				tracepoint();
				(*iter)->setConfig(name, table[name]);
				break;
			}
		}

		// 统一记录日志
		//CLog::instance()->append("RecallConfig", 0, user);

		tracef("CConfigManager::setConfig set all Configs.\n");

		return ret;
	}

	CGuard guard(m_mutex);

	if(m_configAll[name] == table)
	{
		tracepoint();
		return ret;
	}



	//更新全局表格
	m_configAll[name] = table;;
	//m_changed = true;
	m_mutex.Enter();
	m_changed = true;
	saveFile();
	m_mutex.Leave();
	//保存文件,优先处理延迟保存
	//if (!(ret & applyDelaySave))
	//{
	//	saveFile();
	//}
/*
	if (!(ret & IConfigManager::applyWithoutLog))
	{
		ILog::instance()->append("SaveConfig", name, user);
	}
*/
	return ret;
}

void CConfigManager::reg(ISetConfig* p)
{
	svec.push_back(p);
}

void CConfigManager::saveFile()
{
	CGuard guard(m_mutex);

	CConfigWriter writer;

	if(!m_changed)
	{
		return;
	}

	m_changed = false;

	m_stream = "";
	m_stream = writer.write(m_configAll);
	m_fileConfig.Open(m_mainFilePath.c_str(), CFile::modeWrite | CFile::modeCreate);
	if (m_stream.size() != m_fileConfig.Write((char*)m_stream.c_str(), m_stream.size()))
	{
		errorf("CConfigManager::saveFile write main config file failed!\n");
	}
	tracepoint();
	m_fileConfig.Flush();
	m_fileConfig.Close();

}


bool CConfigManager::getDefault(const char* name, CConfigTable& table)
{
	CGuard guard(m_mutex);

	table = m_configDefault[name];
	return table.type() != Json::nullValue;
}

void CConfigManager::config(const char* mainPath, const char* defaultPath)
{
	if ((NULL == mainPath) || (NULL == defaultPath))
	{
		return;
	}

	m_mainFilePath		= mainPath;
	m_defaultFilePath	= defaultPath;
}


bool copyFile(const char* from, const char* to)
{
	CFile file1, file2;
	bool ret = false;

	unsigned char * buf = 0;

	buf = file1.Load(from);

    if(buf)
	{
		if(file2.Open(to, CFile::modeWrite | CFile::modeCreate))
		{
			if(file2.Write(buf, file1.GetLength()) == file1.GetLength())
			{
				ret = true;
			};
			file2.Flush();
			file2.Close();
		}
		file1.UnLoad();
	}

	return ret;
}

void IConfigManager::config(const char* mainPath, const char* defaultPath)
{
	CConfigManager::config(mainPath, defaultPath);
}

IConfigManager *IConfigManager::instance()
{
	return CConfigManager::instance();
}


#include "base/sy_types.h"
#include "base/sy_debug.h"
#include "Manager/sy_file.h"

#include "sy_configManager.h"
#include "Device/sy_device.h"

#include "sy_upgrade.h"

bool IUpgrade::fileChecked(void)
{
	return flag;
}
bool IUpgrade::putfile(const char* file)
{
	
	infof("----%s-------\n", file);
	FILE* fp;
	char buf[128]="";

	sprintf(buf, "unzip -o %s -d /tmp", file);
	infof("buf is [%s]\n", buf);

	flag = false;

	if((fp = popen(buf, "r")) == NULL)
	{
		tracepoint();
		return false;
	}
	pclose(fp);

	CConfigReader reader;
	CFile	m_fileConfig;

	tracepoint();

	if (!m_fileConfig.Open("/tmp/Install", CFile::modeRead | CFile::modeNoTruncate))
	{
		tracepoint();
		return false;
	}

	flag = true;

	const int size = 32*1024;
	char* cbuf = new char[size + 1];

	std::string input="";
	
	IDevice::instance()->setLed(IDevice::LED_UPDATE, 2);

	while (1)
	{
		int nLen = m_fileConfig.Read(cbuf, size);

		if(nLen <=0 )
			break;
		cbuf[nLen] = 0;
		input += cbuf;
	}
	input += '\0';
	m_fileConfig.Close();
	delete []cbuf;

	tracepoint();

	CConfigTable table;
	std::string cmd="";
	if(reader.parse(input, table))
	{
		//std::cout << table["Commands"] << std::endl;
		for(int i=0;i<table["Commands"].size();i++)
		{
			cmd = table["Commands"][i].asString();
			std::cout << cmd << std::endl;
			if((fp = popen(cmd.c_str(), "r")) == NULL)
				return false;

			bzero(buf, sizeof(buf));
			while(fgets(buf, sizeof(buf), fp)!=NULL)
			{
				infof("%d-[%s]\n", i, buf);
				bzero(buf, sizeof(buf));
			}
			pclose(fp);
		}

	}

	sleep(3);
	system("reboot");


	return true;

#if 0
	if((fp = popen("umount /app", "r")) == NULL)
		return false;
	
	fgets(buf, sizeof(buf), fp);
	infof("1-[%s]\n", buf);
	fclose(fp);

	if((fp = popen("flash_eraseall /dev/mtd4", "r")) == NULL)
		return false;

	bzero(buf, sizeof(buf));
	fgets(buf, sizeof(buf), fp);
	infof("2-[%s]\n", buf);
	fclose(fp);

	bzero(buf, sizeof(buf));
	sprintf(buf, "nandwrite -a -o /dev/mtd4 %s", file);
	infof("cmd:[%s]\n", buf);
	if((fp = popen(buf, "r")) == NULL)
		return false;
	
	bzero(buf, sizeof(buf));
	fgets(buf, sizeof(buf), fp);
	infof("3-[%s]\n", buf);
	fclose(fp);

	system("reboot");
	return true;
#endif
}
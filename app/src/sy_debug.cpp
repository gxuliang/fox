/*
 * debug.cpp
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */

#include "base/sy_types.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include "base/sy_debug.h"


void logsave(int hour,const char* szcontent)
{
	static int mhour = 0;
	static int fd = 0;

	if(access("/config/rc.d/K02", R_OK|W_OK) < 0)
	{
		//printf("%s\n", "88888888888888888888888888888");
		return;
	}

	if(mhour == 0 || mhour != hour)
	{
		if(fd > 0)
		{
			close(fd);
			fd = 0;
		}
		fd = open("/config/log", O_WRONLY | O_CREAT);
		mhour = hour;		
	}

	if(fd > 0)
	{
		write(fd, szcontent, strlen(szcontent));
		//write(fd, "\r\n", 2);
	}

}

/////////////////////////////////////////////////////////////////////////
 int sys_debug (const char* mfile, int mline, const char* module, LOG_Lv level, const char* format, ... )
 {
	 char szbuf[1024*8+256] = "", szcontent[1024*8] = "";
	 va_list ap;
	 struct timeval tv;
	 struct tm stm;
	 const char * g_log_lv[] =
	 {
		 "NULL",
		 "TRACE",
		 "DBG",
		 "INFO",
		 "WARN",
		 "ERROR",
		 "FATAL",
	 };

	 const int color[] = {0, 37, 36, 32, 33, 31, 35};

	 va_start ( ap, format );
	 vsprintf ( szcontent, format, ap );
	 va_end ( ap );

	 gettimeofday ( &tv, NULL );
	 stm = * ( localtime ( &tv.tv_sec ) );

	 if(szcontent[strlen(szcontent) - 1] == '\n')
		 szcontent[strlen(szcontent) - 1] = '\0';

	 sprintf ( szbuf, "\033[%d;40m%02d:%02d:%02d[%s-%d][%s] %s\033[0m\n",color[level],
				 stm.tm_hour, stm.tm_min, stm.tm_sec,
				 mfile,mline,module,szcontent );

	 printf ( "%s", szbuf );
	 logsave(stm.tm_hour, szbuf);
	 return 1;

 }

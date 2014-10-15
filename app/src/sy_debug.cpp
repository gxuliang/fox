/*
 * debug.cpp
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */


#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include "base/sy_debug.h"


/////////////////////////////////////////////////////////////////////////
 int sys_debug (const char* module, LOG_Lv level, const char* format, ... )
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

	 sprintf ( szbuf, "\033[%d;40m%02d:%02d:%02d[%s] [%s-%d]%s\033[0m\n",color[level],
				 stm.tm_hour, stm.tm_min, stm.tm_sec,
				 module, __FILE__,__LINE__,szcontent );

	 printf ( "%s", szbuf );
	 return 1;

 }

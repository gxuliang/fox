/*
 * sy_types.h
 *
 *  Created on: 2013-3-19
 *      Author: xul
 */

#ifndef SY_TYPES_H_
#define SY_TYPES_H_



#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

/*! standard C library */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

/*! Linux system calls */
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/select.h>
#include <sys/vfs.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>
#include <sys/types.h>
//#include <sys/io.h>
#include <sys/sendfile.h>
#include<linux/reboot.h>
/*! other interfaces */
#include <time.h>
#include <string.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <termios.h>
#include <dirent.h>
#include <crypt.h>
#include <limits.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>



#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include "json/json.h"

//////////////////////////////////////////////////////////////////////////
// base types definition
// compilers should be restricted to ensure the following equalities!
typedef signed char				schar;	///< sizeof(uchar) == sizeof(schar) == sizeof(char) == 1
typedef unsigned char			uchar;
typedef unsigned int			uint;	///< sizeof(uint) == sizeof(int) == 4
typedef unsigned short			ushort;	///< sizeof(ushort) == sizeof(short) == 2
typedef unsigned long			ulong;	///< sizeof(ulong) == sizeof(long) == 4
#ifdef WIN32
typedef __int64					int64;	///< sizeof(int64) == sizeof(uint64) == 8
typedef unsigned __int64		uint64;
#elif defined(__GNUC__)
typedef long long				int64;
typedef unsigned long long		uint64;
#elif defined(__TCS__)
typedef signed   long long int	int64;
typedef unsigned long long int	uint64;
#endif

typedef Json::Value CConfigTable;

#define MAX_PATH				512

#endif /* SY_TYPES_H_ */

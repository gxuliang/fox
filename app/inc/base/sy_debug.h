/*
 * sy_debug.h
 *
 *  Created on: 2013-3-18
 *      Author: xul
 */

#ifndef SY_DEBUG_H_
#define SY_DEBUG_H_

#define FUNC    (__FUNCTION__)
#define IFILE    (__FILE__)

enum LOG_Lv
{
    LOG_NONE,
    LOG_TRACE,
    LOG_DBG,
    LOG_MSG,
    LOG_WARN,
    LOG_ERR,
    LOG_FATAL,
};
int sys_debug (const char* module, LOG_Lv level, const char* format, ... );

#define tracef(format, ...) sys_debug (FUNC, LOG_TRACE, format, ## __VA_ARGS__)
#define debugf(format, ...) sys_debug (FUNC, LOG_DBG, format, ## __VA_ARGS__)
#define infof(format, ...) sys_debug (FUNC, LOG_MSG, format, ## __VA_ARGS__)
#define warnf(format, ...) sys_debug (FUNC, LOG_WARN, format, ## __VA_ARGS__)
#define errorf(format, ...) sys_debug (FUNC, LOG_ERR, format, ## __VA_ARGS__)
#define fatalf(format, ...) sys_debug (FUNC, LOG_FATAL, format, ## __VA_ARGS__)

#define tracepoint()  debugf("tracepoint: %s, %d.\n",__FILE__,__LINE__)
#endif /* SY_DEBUG_H_ */

/*
 *	@file log.h
 *	@author mateuszpiesta (mateusz.piesta@mudita.com)
 *	@date 5 cze 2018
 *  @brief Generic logging utilities.
 *  This module is wrapper over printf with additional thread safety provided and common logs levels.
 *
 *  USAGE:
 *
 *  Include this header in any file that wishes to write to logger(s).  In
 *  exactly one file (per executable), define LOG_MAIN first (e.g. in your
 *  main .c file).
 *
 *     #define LOG_MAIN
 *     #include "logs/log.h"
 *
 *  This will define the actual objects that all the other units will use.
 *  Then invoke log_Init(VFS_FILE* fp,logger_level level) function to initialize logger utility. If you want
 *additionally store logs into file pass valid file descriptor to init function. By default logger logs info into STDOUT
 *stream. After that logger is ready to use.
 *
 *  Examples:
 *  1)
 *   log_Init(NULL,LOGDEBUG);
 *   Send logs(level higher or equal to LOGDEBUG) to STDOUT stream.
 *
 *  2)
 *   log_Init('valid file pointer',LOGINFO);
 *   Send logs(level higher or equal to LOGINFO) to STDOUT stream and also to file specified by user.
 */

#ifndef LOG_LOG_H_
#define LOG_LOG_H_

#include "debug.hpp"
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define RTT_LUART RTT_LUART

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        LOGTRACE,
        LOGDEBUG,
        LOGINFO,
        LOGWARN,
        LOGERROR,
        LOGFATAL
    } logger_level;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

    /**
     * Forward declarations
     */
    void log_Log(logger_level level, const char *file, int line, const char *function, const char *fmt, ...)
        __attribute__((format(printf, 5, 6)));
    void log_SetLevel(logger_level level);
    void log_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
    void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes);

/**
 * Log functions (one per level).
 */
#ifdef UNIT_TESTS
#define LOG_PRINTF(...)
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)
#define LOG_CUSTOM(...)
#else
#define LOG_PRINTF(...)              log_Printf(__VA_ARGS__)
#define LOG_TRACE(...)               log_Log(LOGTRACE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)               log_Log(LOGDEBUG, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)                log_Log(LOGINFO, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...)                log_Log(LOGWARN, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...)               log_Log(LOGERROR, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_FATAL(...)               log_Log(LOGFATAL, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_CUSTOM(loggerLevel, ...) log_Log(loggerLevel, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_LOG_H_ */

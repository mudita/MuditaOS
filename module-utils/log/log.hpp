// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
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
    int log_Log(logger_level level, const char *file, int line, const char *function, const char *fmt, ...)
        __attribute__((format(printf, 5, 6)));
    int log_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
    void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes);

/**
 * Log functions (one per level).
 */
#define LOG_PRINTF(...)              log_Printf(__VA_ARGS__)
#define LOG_TRACE(...)               log_Log(LOGTRACE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)               log_Log(LOGDEBUG, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)                log_Log(LOGINFO, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...)                log_Log(LOGWARN, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...)               log_Log(LOGERROR, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_FATAL(...)               log_Log(LOGFATAL, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_CUSTOM(loggerLevel, ...) log_Log(loggerLevel, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#if LOG_SENSITIVE_DATA_ENABLED
#define LOG_SENSITIVE(loggerLevel, ...) log_Log(loggerLevel, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_SENSITIVE(loggerLevel, ...)
#endif

#ifdef __cplusplus
}
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static const size_t LOGGER_BUFFER_SIZE  = 8192;
static const char *LOG_FILE_NAME        = "MuditaOS.log";
static const int MAX_LOG_FILES_COUNT    = 3;
static const size_t MAX_LOG_FILE_SIZE   = 1024 * 1024 * 15; // 15 MB
#pragma GCC diagnostic pop

#endif /* LOG_LOG_H_ */

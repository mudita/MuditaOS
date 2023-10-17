// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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
    } LoggerLevel;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

    /**
     * Forward declarations
     */
    int log_Log(LoggerLevel level, const char *file, int line, const char *function, const char *fmt, ...)
        __attribute__((format(printf, 5, 6)));
#ifdef LOG_IGNORE_ALL
    int log_ignore(logger_level level, const char *file, int line, const char *function, const char *fmt, ...)
        __attribute__((format(printf, 5, 6)));
#endif
    int log_Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
    void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes);
    size_t log_getMaxLineLength();

/**
 * Log functions (one per level).
 */
#define LOG_PRINTF(...) log_Printf(__VA_ARGS__)
#ifndef LOG_IGNORE_ALL
#define LOG_TRACE(...)               log_Log(LOGTRACE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)               log_Log(LOGDEBUG, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)                log_Log(LOGINFO, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...)                log_Log(LOGWARN, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...)               log_Log(LOGERROR, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_FATAL(...)               log_Log(LOGFATAL, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_CUSTOM(loggerLevel, ...) log_Log(loggerLevel, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_TRACE(...)               log_ignore(LOGTRACE, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)               log_ignore(LOGDEBUG, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)                log_ignore(LOGINFO, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...)                log_ignore(LOGWARN, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...)               log_ignore(LOGERROR, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_FATAL(...)               log_ignore(LOGFATAL, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#define LOG_CUSTOM(loggerLevel, ...) log_ignore(loggerLevel, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#endif

#if LOG_SENSITIVE_DATA_ENABLED
#define LOG_SENSITIVE(loggerLevel, ...) log_Log(loggerLevel, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_SENSITIVE(loggerLevel, ...)
#endif

#ifdef __cplusplus
}
#endif

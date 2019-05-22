//
// Created by mati on 08.05.19.
//

#include "../../log/log.hpp"
#include "../../log/segger/SEGGER_RTT.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <assert.h>


#define LOGGER_BUFFER_SIZE  4096

#define CONSOLE_ESCAPE_COLOR_BLACK              "\x1b[30m"
#define CONSOLE_ESCAPE_COLOR_RED                "\x1b[31m"
#define CONSOLE_ESCAPE_COLOR_GREEN              "\x1b[32m"
#define CONSOLE_ESCAPE_COLOR_YELLOW             "\x1b[33m"
#define CONSOLE_ESCAPE_COLOR_BLUE               "\x1b[34m"
#define CONSOLE_ESCAPE_COLOR_MAGENTA            "\x1b[35m"
#define CONSOLE_ESCAPE_COLOR_CYAN               "\x1b[36m"
#define CONSOLE_ESCAPE_COLOR_WHITE              "\x1b[37m"
#define CONSOLE_ESCAPE_COLOR_RESET              "\x1b[0m"


#define CONSOLE_ESCAPE_COLOR_BACKGROUND_BLACK   "\x1b[40m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_RED     "\x1b[41m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_GREEN   "\x1b[42m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_YELLOW  "\x1b[43m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_BLUE    "\x1b[44m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_MAGENTA "\x1b[45m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_CYAN    "\x1b[46m"
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_WHITE   "\x1b[47m"

const char *level_names[] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#if LOG_USE_COLOR == 1
static const char *level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif

struct Logger{
    Logger( logger_level level = LOGTRACE ) : level{ level } {

        lock = xSemaphoreCreateMutex();

        assert(lock != NULL);

        /* Acquire lock */
        if(xSemaphoreTake(lock,100) != pdPASS){
            return;
        }

        level = level;

        /* Release lock */
        xSemaphoreGive(lock);

    }

    xSemaphoreHandle lock;
    logger_level level;
};

static Logger logger;
static char loggerBuffer[LOGGER_BUFFER_SIZE] = {0};


void log_Printf(const char *fmt, ...)
{
    /* Acquire lock */
    if(xSemaphoreTake(logger.lock,100) != pdPASS){
        return;
    }
    char* ptr = loggerBuffer;
    va_list args;

    va_start(args, fmt);
    ptr += vsnprintf(ptr,&loggerBuffer[LOGGER_BUFFER_SIZE]-ptr, fmt, args);
    va_end(args);

    SEGGER_RTT_Write(0,(uint8_t*)loggerBuffer,ptr-loggerBuffer);

    /* Release lock */
    xSemaphoreGive(logger.lock);
}

void log_Log(logger_level level, const char *file, int line,const char *function, const char *fmt, ...)
{
    /* Acquire lock */
    if(xSemaphoreTake(logger.lock,100) != pdPASS){
        return;
    }

    char* ptr = loggerBuffer;

    va_list args;
#if LOG_USE_COLOR == 1
    ptr += sprintf(ptr,"%s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
        level_colors[level], level_names[level], file, line);
#else
    ptr += sprintf(ptr,"%-5s %s:%s:%d: ", level_names[level], file, function, line);
#endif
    va_start(args, fmt);
    ptr += vsnprintf(ptr,&loggerBuffer[LOGGER_BUFFER_SIZE]-ptr, fmt, args);
    va_end(args);
    ptr += sprintf(ptr,"\r\n");

    SEGGER_RTT_Write(0,loggerBuffer,ptr-loggerBuffer);

    /* Release lock */
    xSemaphoreGive(logger.lock);
}

/**
 * Update log level
 * @param level [in] - new log level
 */
void log_SetLevel(logger_level level) {
    logger.level = level;
}
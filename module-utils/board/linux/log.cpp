//
// Created by mati on 08.05.19.
//

#include "../../log/log.hpp"
#include <iostream>
#include <string>
#include <mutex>
#include <ticks.hpp>

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
    }
    std::mutex lock;
    logger_level level;
};

static Logger logger;
static char loggerBuffer[LOGGER_BUFFER_SIZE] = {0};


void log_Printf(const char *fmt, ...)
{
    /* Acquire lock */
    std::lock_guard<std::mutex> guard(logger.lock);
    char* ptr = loggerBuffer;
    ptr[0] = 0;
    va_list args;

    va_start(args, fmt);
    vsnprintf(ptr,LOGGER_BUFFER_SIZE-1, fmt, args);
    va_end(args);

    std::cout<<loggerBuffer;
}

static void _log_Log(logger_level level, const char *file, int line,const char *function, const char *fmt, va_list args)
{
    if (level < logger.level) {
        return;
    }

    std::lock_guard<std::mutex> guard(logger.lock);

    char* ptr = loggerBuffer;

    ptr += sprintf(ptr, "%d ms ", cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

#if LOG_USE_COLOR == 1
    ptr += sprintf(ptr,"%s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
        level_colors[level], level_names[level], file, line);
#else
    ptr += sprintf(ptr,"%-5s %s:%s:%d: ", level_names[level], file, function, line);
#endif
    ptr += vsnprintf(ptr,&loggerBuffer[LOGGER_BUFFER_SIZE]-ptr, fmt, args);
    ptr += sprintf(ptr,"\r\n");

    std::cout<<loggerBuffer;
}

void log_Log(logger_level level, const char *file, int line,const char *function, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _log_Log(level, file, line,function, fmt, args);
    va_end(args);
}

/**
 * Update log level
 * @param level [in] - new log level
 */
void log_SetLevel(logger_level level) {
    logger.level = level;
}

extern "C" {

    void bt_log_custom(const char* file , int line, const char* foo, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        _log_Log( LOGTRACE, file, line, foo, fmt, args);
        va_end(args);
      }

};

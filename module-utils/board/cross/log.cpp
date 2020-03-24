//
// Created by mati on 08.05.19.
//

#include "../../log/log.hpp"
#include "../../segger/rtt/SEGGER_RTT.h"
#include <ticks.hpp>
extern "C"
{
#include "FreeRTOS.h"
}
#include "semphr.h"
#include <assert.h>
#include <board.h>
#include <critical.hpp>

/*
 * TODO: M.P
 * Redictering log output to serial console in this form need be considered as quick&dirty solution.
 * It should be refactored i.e serial terminal BSP created and used here instead of using RT1051's low-level routines
 * directly.
 */

#define LOGGER_BUFFER_SIZE 4096

#if LOG_USE_COLOR == 1
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
#else
#define CONSOLE_ESCAPE_COLOR_BLACK
#define CONSOLE_ESCAPE_COLOR_RED
#define CONSOLE_ESCAPE_COLOR_GREEN
#define CONSOLE_ESCAPE_COLOR_YELLOW
#define CONSOLE_ESCAPE_COLOR_BLUE
#define CONSOLE_ESCAPE_COLOR_MAGENTA
#define CONSOLE_ESCAPE_COLOR_CYAN
#define CONSOLE_ESCAPE_COLOR_WHITE
#define CONSOLE_ESCAPE_COLOR_RESET
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_BLACK
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_RED
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_GREEN
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_YELLOW
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_BLUE
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_MAGENTA
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_CYAN
#define CONSOLE_ESCAPE_COLOR_BACKGROUND_WHITE
#endif

const char *level_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#if LOG_USE_COLOR == 1
static const char *level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#else
static const char *level_colors[] = {"", "", "", "", "", ""};
#endif

#include <map>
#include <string>

struct Logger
{
    Logger(logger_level level = LOGTRACE) : level{level}
    {

        lock = xSemaphoreCreateMutex();

        assert(lock != NULL);

        /* Acquire lock */
        if (xSemaphoreTake(lock, 100) != pdPASS) {
            return;
        }

        level = level;

        /* Release lock */
        xSemaphoreGive(lock);
    }

    bool logLock()
    {
        // if called from the ISR use DI/EI. In all other cases use semaphore.
        if (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) {
            bt = cpp_freertos::CriticalSection::EnterFromISR();
        }
        else if (xSemaphoreTake(lock, 100) != pdPASS) {
            return false;
        }
        return true;
    }

    void logUnlock()
    {
        // if called from the ISR use DI/EI. In all other cases use semaphore.
        if (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) {
            cpp_freertos::CriticalSection::ExitFromISR(bt);
        }
        else {
            xSemaphoreGive(lock);
        }
    }

    std::map<std::string, logger_level> filtered = {
        // {"ServiceDB", logger_level::LOGFATAL},
        {"ApplicationManager", logger_level::LOGINFO},
    };

    /// Filter out not interesting logs via thread Name
    /// its' using fact that:
    /// - TRACE is level 0, for unedfined lookups it will be alvways trace
    /// - it will be one time init for apps which doesn't tell what level they should have
    /// - for others it will be o1 lookup so it's fine
    logger_level filterThreadName(const std::string &name)
    {
        return filtered[name];
    }

    xSemaphoreHandle lock;
    logger_level level;
    BaseType_t bt;
};

static Logger logger;
static char loggerBuffer[LOGGER_BUFFER_SIZE] = {0};

void log_Printf(const char *fmt, ...)
{
    if (!logger.logLock()) {
        return;
    }

    char *ptr = loggerBuffer;
    va_list args;

    va_start(args, fmt);
    ptr += vsnprintf(ptr, &loggerBuffer[LOGGER_BUFFER_SIZE] - ptr, fmt, args);
    va_end(args);

    log_WriteToDevice((uint8_t *)loggerBuffer, ptr - loggerBuffer);

    logger.logUnlock();
}

/// get string description:
/// - in critical seciton - return CRIT
/// - in interrupt return - IRQ
/// - else return thread name
static inline const char *getTaskDesc()
{
    return xTaskGetCurrentTaskHandle() == NULL
               ? "CRIT"
               : xPortIsInsideInterrupt() ? "IRQ" : pcTaskGetName(xTaskGetCurrentTaskHandle());
}

static void _log_Log(
    logger_level level, const char *file, int line, const char *function, const char *fmt, va_list args)
{
    if (!logger.logLock()) {
        return;
    }

    char *ptr = loggerBuffer;

    // filter out not interesing logs
    if (logger.filterThreadName(getTaskDesc()) >= level) {
        logger.logUnlock();
        return;
    }

    ptr += sprintf(ptr, "%lu ms ", cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));

#if LOG_USE_COLOR == 1

    ptr += sprintf(ptr,
                   "%s%-5s " CONSOLE_ESCAPE_COLOR_MAGENTA "[%-10s] \x1b[90m%s:%d:" CONSOLE_ESCAPE_COLOR_RESET,
                   level_colors[level],
                   level_names[level],
                   getTaskDesc(),
                   file,
                   line);
#else
    ptr += sprintf(ptr, "%-5s %s:%s:%d: ", level_names[level], file, function, line);
#endif
    ptr += vsnprintf(ptr, &loggerBuffer[LOGGER_BUFFER_SIZE] - ptr, fmt, args);
    ptr += sprintf(ptr, "\r\n");

    log_WriteToDevice((uint8_t *)loggerBuffer, ptr - loggerBuffer);

    /* Release lock */
    logger.logUnlock();
}

void log_Log(logger_level level, const char *file, int line, const char *function, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _log_Log(level, file, line, function, fmt, args);
    va_end(args);
}

/**
 * Update log level
 * @param level [in] - new log level
 */
void log_SetLevel(logger_level level)
{
    logger.level = level;
}

extern "C"
{

    void bt_log_custom(const char *file, int line, const char *foo, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        _log_Log(LOGTRACE, file, line, foo, fmt, args);
        va_end(args);
    }

    int printf(__const char *__restrict __format, ...)
    {
        /* Acquire lock */
        if (!logger.logLock()) {
            return -1;
        }

        char *ptr = loggerBuffer;
        va_list args;

        va_start(args, __format);
        ptr += vsnprintf(ptr, &loggerBuffer[LOGGER_BUFFER_SIZE] - ptr, __format, args);
        va_end(args);

        unsigned int numBytes = ptr - loggerBuffer;
        SEGGER_RTT_Write(0, (uint8_t *)loggerBuffer, numBytes);

        /* Release lock */
        logger.logUnlock();

        return numBytes;
    }

    int vprintf(const char *__restrict __format, va_list __arg)
    {
        /* Acquire lock */
        if (!logger.logLock()) {
            return -1;
        }
        char *ptr = loggerBuffer;
        ptr += vsnprintf(ptr, &loggerBuffer[LOGGER_BUFFER_SIZE] - ptr, __format, __arg);

        unsigned int numBytes = ptr - loggerBuffer;
        SEGGER_RTT_Write(0, (uint8_t *)loggerBuffer, numBytes);

        /* Release lock */
        logger.logUnlock();

        return numBytes;
    }

    int log_assert(__const char *__restrict __format, ...)
    {
        /* Acquire lock */
        if (!logger.logLock()) {
            return -1;
        }

        char *ptr = loggerBuffer;
        ptr += sprintf(ptr, "%lu ms ", cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
        logger_level level = LOGFATAL;
        ptr += sprintf(ptr,
                       "%s%-5s " CONSOLE_ESCAPE_COLOR_MAGENTA "[%-10s] \x1b[31mASSERTION " CONSOLE_ESCAPE_COLOR_RESET,
                       level_colors[level],
                       level_names[level],
                       getTaskDesc());

        va_list args;
        va_start(args, __format);
        ptr += vsnprintf(ptr, &loggerBuffer[LOGGER_BUFFER_SIZE] - ptr, __format, args);
        va_end(args);

        unsigned int numBytes = ptr - loggerBuffer;

        log_WriteToDevice((uint8_t *)loggerBuffer, ptr - loggerBuffer);

        /* Release lock */
        logger.logUnlock();

        return numBytes;
    }

    void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
    {
        log_assert("\"%s\" failed: file \"%s\", line %d%s%s\n",
                   failedexpr,
                   file,
                   line,
                   func ? ", function: " : "",
                   func ? func : "");

        abort();
        /* NOTREACHED */
    }
};

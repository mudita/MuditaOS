// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <FreeRTOS.h>
#include <MIMXRT1051.h>
#include <log/log.hpp>
#include <logdump/logdump.h>
#include <task.h>
#include <macros.h>
#include <string.h>
#include <exit_backtrace.h>
#include <purefs/vfs_subsystem.hpp>

static void __attribute__((noreturn)) stop_system(void)
{
    if (!isIRQ()) {
        if (shutdownFlushLogs() != 1) {
            LOG_ERROR("Cannot dump logs");
        }
        const auto err = purefs::subsystem::unmount_all();
        if (err) {
            LOG_WARN("Unable unmount all filesystems with error: %i.", err);
        }
        else {
            LOG_INFO("Filesystems unmounted successfully...");
        }
    }
    vTaskSuspendAll();
    haltIfDebugging();
    LOG_INFO("Restarting the system...");
    NVIC_SystemReset();
    // waiting for system reset
    while (true) {
#ifndef DEBUG
        __asm volatile("wfi\n");
#endif
    };
}

void __attribute__((noreturn, used)) _exit_backtrace(int code, bool bt_dump)
{
    if (bt_dump && !isIRQ()) {
        _StackTrace_Dump_And_Abort();
    }
    stop_system();
};

void __attribute__((noreturn, used)) _exit(int code)
{
    _exit_backtrace(code, code != 0);
}

void __attribute__((noreturn, used)) exit(int code)
{
    // do not call global destructors and atexit registered functions
    _exit(code);
}

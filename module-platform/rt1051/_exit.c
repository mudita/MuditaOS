// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// ----------------------------------------------------------------------------

#include <FreeRTOS.h>
#include <MIMXRT1051.h>
#include <log.hpp>
#include <task.h>
#include <macros.h>
// ----------------------------------------------------------------------------

// Forward declaration

void
_exit(int code);

// ----------------------------------------------------------------------------

// On Release, call the hardware reset procedure.
// On Debug we just enter an infinite loop, to be used as landmark when halting
// the debugger.
//
// It can be redefined in the application, if more functionality
// is required.

void __attribute__((weak)) _exit(int code)
{
    LOG_FATAL("_exit %d", code);
    haltIfDebugging();
    vTaskEndScheduler();
#ifdef DEBUG
    while(1){};
#endif
}

// ----------------------------------------------------------------------------


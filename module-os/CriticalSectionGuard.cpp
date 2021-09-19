// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CriticalSectionGuard.hpp"

#include <board/macros.h>

extern "C"
{
#include <FreeRTOS.h>
#include <portmacro.h>
}

using namespace cpp_freertos;

CriticalSectionGuard::CriticalSectionGuard()
{
    if (isIRQ()) {
        savedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    }
    else {
        vPortEnterCritical();
    }
}

CriticalSectionGuard::~CriticalSectionGuard()
{
    if (isIRQ()) {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(savedInterruptStatus);
    }
    else {
        vPortExitCritical();
    }
}

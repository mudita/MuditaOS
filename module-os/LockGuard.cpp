// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "critical.hpp"
#include "LockGuard.hpp"
#include <macros.h>
#include <stdexcept>

namespace
{
    [[nodiscard]] bool isOSRunning()
    {
        return xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
    }
} // namespace

LockGuard::LockGuard(cpp_freertos::MutexStandard &mutex) : mutex(mutex)
{
    if (not isOSRunning()) {
        return;
    }
    if (isIRQ()) {
        savedInterruptStatus = cpp_freertos::CriticalSection::EnterFromISR();
    }
    else if (!mutex.Lock()) {
        throw std::runtime_error("LockGuard failed to lock mutex");
    }
}

LockGuard::~LockGuard()
{
    if (not isOSRunning()) {
        return;
    }
    if (isIRQ()) {
        cpp_freertos::CriticalSection::ExitFromISR(savedInterruptStatus);
    }
    else {
        mutex.Unlock();
    }
}

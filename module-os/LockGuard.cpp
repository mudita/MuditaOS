// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "critical.hpp"
#include "LockGuard.hpp"
#include <macros.h>
#include <stdexcept>

LockGuard::LockGuard(cpp_freertos::MutexStandard& mutex) : mutex(mutex)
{
    if (isIRQ()) {
        savedInterruptStatus = cpp_freertos::CriticalSection::EnterFromISR();
    }
    else if(!mutex.Lock()) {
        throw std::runtime_error("LockGuard failed to lock mutex");
    }
}

LockGuard::~LockGuard()
{
    if (isIRQ()) {
        cpp_freertos::CriticalSection::ExitFromISR(savedInterruptStatus);
    }
    else {
        mutex.Unlock();
    }
}

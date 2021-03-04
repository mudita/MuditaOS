// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <mutex.hpp>

class LockGuard
{
public:
    explicit LockGuard(cpp_freertos::MutexStandard& mutex);
    ~LockGuard();
    LockGuard(const LockGuard &) = delete;
    LockGuard(LockGuard &&)      = delete;

    LockGuard &operator=(const LockGuard &) = delete;
    LockGuard &operator=(LockGuard &&) = delete;

private:
    BaseType_t savedInterruptStatus;
    cpp_freertos::MutexStandard &mutex;
};

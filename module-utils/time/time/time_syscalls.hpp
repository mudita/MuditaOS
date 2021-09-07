// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <sys/time.h>

namespace utils::internal::syscalls
{
    int gettimeofday(int &_errno_, struct timeval *tp, void *tzp);
    void tz_lock();
    void tz_unlock();
    void env_lock(int &_errno_);
    void env_unlock(int &_errno_);
} // namespace utils::internal::syscalls

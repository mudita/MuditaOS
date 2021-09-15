// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <time/time_syscalls.hpp>
#include <bsp/rtc/rtc.hpp>
#include <mutex.hpp>
#include <errno.h>
#include <cstring>

namespace utils::internal::syscalls
{
    namespace
    {
        cpp_freertos::MutexRecursive g_time_lock;
        cpp_freertos::MutexRecursive g_env_lock;
    } // namespace

    int gettimeofday(int &_errno_, struct timeval *tp, void *tzp)
    {
        if (!tp) {
            _errno_ = EINVAL;
            return -1;
        }
        time_t curtime;
        auto err = bsp::rtc::getCurrentTimestamp(&curtime);
        if (err != bsp::rtc::ErrorCode::OK) {
            _errno_ = EIO;
            return -1;
        }
        tp->tv_sec  = curtime;
        tp->tv_usec = 0;
        return 0;
    }
    void tz_lock()
    {
        g_time_lock.Lock();
    }
    void tz_unlock()
    {
        g_time_lock.Unlock();
    }

    void env_lock(int &_errno_)
    {
        if (!g_env_lock.Lock()) {
            _errno_ = ENXIO;
        }
    }
    void env_unlock(int &_errno_)
    {
        if (!g_env_lock.Unlock()) {
            _errno_ = ENXIO;
        }
    }
} // namespace utils::internal::syscalls

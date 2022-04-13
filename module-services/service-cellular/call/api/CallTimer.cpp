// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallTimer.hpp"
#include <Timers/TimerFactory.hpp>

CallTimer::CallTimer(sys::TimerHandle handle) : handle(std::move(handle))
{}

void CallTimer::start()
{
    startActiveTime = std::time(nullptr);
    handle.start();
}

void CallTimer::stop()
{
    handle.stop();
}

time_t CallTimer::duration()
{
    return std::time(nullptr) - startActiveTime;
}

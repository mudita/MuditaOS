// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallTimer.hpp"

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
    const auto timeElapsed = std::time(nullptr) - startActiveTime;
    return handle.isActive() ? timeElapsed : 0;
}

TimerRing::TimerRing(sys::TimerHandle handle) : handle(std::move(handle))
{}

void TimerRing::start()
{
    handle.start();
}

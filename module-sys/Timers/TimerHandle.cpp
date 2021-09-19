// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimerHandle.hpp"
#include <log.hpp>

namespace sys
{
    TimerHandle::TimerHandle(Timer *timer) noexcept : timer{timer}
    {}

    TimerHandle::TimerHandle(TimerHandle &&oth) noexcept
    {
        reset(oth.timer);
        oth.timer = nullptr;
    }

    TimerHandle &TimerHandle::operator=(TimerHandle &&oth) noexcept
    {
        if (this != &oth) {
            reset(oth.timer);
            oth.timer = nullptr;
        }
        return *this;
    }

    bool TimerHandle::isValid() const noexcept
    {
        return timer != nullptr;
    }

    void TimerHandle::reset(Timer *newTimer) noexcept
    {
        delete timer;
        timer = newTimer;
    }

    TimerHandle::~TimerHandle() noexcept
    {
        if (isValid()) {
            timer->stop();
        }
        reset();
    }

    void TimerHandle::start()
    {
        if (isValid()) {
            timer->start();
        }
        else {
            LOG_ERROR("Cannot start, null timer pointer");
        }
    }

    void TimerHandle::stop()
    {
        if (isValid()) {
            timer->stop();
        }
        else {
            LOG_ERROR("Cannot stop, null timer pointer");
        }
    }

    void TimerHandle::restart(std::chrono::milliseconds newInterval)
    {
        if (isValid()) {
            timer->restart(newInterval);
        }
        else {
            LOG_ERROR("Cannot restart, null timer pointer");
        }
    }

    bool TimerHandle::isActive() const noexcept
    {
        return isValid() ? timer->isActive() : false;
    }
} // namespace sys

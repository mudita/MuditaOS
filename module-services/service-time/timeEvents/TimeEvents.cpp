// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeEvents.hpp"

#include <utility> // for move

#include "Common/Query.hpp"  // for QueryResult
#include "Service/Timer.hpp" // for Timer, Timer::Type, Timer::Type::SingleShot

namespace sys
{
    class Service;
} // namespace sys

namespace stm
{
    constexpr static const int eventTimerInitInterval = 1000;

    TimeEvents::TimeEvents(sys::Service *service) : serv(service)
    {}

    TimeEvents::~TimeEvents()
    {
        stopTimer();
    }

    std::unique_ptr<sys::Timer> &TimeEvents::timer()
    {
        if (fireEventTimer == nullptr) {
            fireEventTimer = std::make_unique<sys::Timer>(
                timerName(), service(), eventTimerInitInterval, sys::Timer::Type::SingleShot);
        }
        return fireEventTimer;
    }

    void TimeEvents::startProcessing()
    {
        timersProcessingStarted = true;
        processNextEvent();
    }

    void TimeEvents::stopProcessing()
    {
        stopTimer();
        timersProcessingStarted = false;
    }

    void TimeEvents::processNextEvent()
    {
        stopTimer();
        if (!isStarted()) {
            return;
        }

        sendNextEventQuery();
    }

    void TimeEvents::stopTimer()
    {
        timer()->stop();
    }

    void TimeEvents::recreateTimer(uint32_t interval)
    {
        stopTimer();
        if (!isStarted()) {
            return;
        }

        timer()->connect([=](sys::Timer &) { fireEventTimerCallback(); });
        timer()->reload(interval);
    }

    void TimeEvents::fireEventTimerCallback()
    {
        if (!isStarted()) {
            return;
        }

        invokeEvent();
        sendEventFiredQuery();
    }

    bool TimeEvents::receiveNextEventQuery(std::unique_ptr<db::QueryResult> nextEventQueryResult)
    {
        if (!isStarted()) {
            return true;
        }
        if (nextEventQueryResult == nullptr) {
            return false;
        }

        uint32_t interval = calcToNextEventInterval(std::move(nextEventQueryResult));
        if (interval > 0) {
            recreateTimer(interval);
        }
        return true;
    }
} // namespace stm

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/TimeEvents.hpp>

#include <Common/Query.hpp>
#include <module-sys/Timers/TimerFactory.hpp>

#include <utility>

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

    sys::TimerHandle &TimeEvents::timer()
    {
        if (!fireEventTimer.isValid()) {
            fireEventTimer = sys::TimerFactory::createSingleShotTimer(
                service(), timerName(), std::chrono::milliseconds{eventTimerInitInterval}, [this](sys::Timer &) {
                    fireEventTimerCallback();
                });
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
        timer().stop();
    }

    void TimeEvents::recreateTimer(uint32_t interval)
    {
        stopTimer();
        if (!isStarted()) {
            return;
        }

        timer().restart(std::chrono::milliseconds{interval});
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

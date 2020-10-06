#include "TimeEvents.hpp"

namespace stm
{
    constexpr static const int eventTimerInitInterval = 1000;

    TimeEvents::TimeEvents(sys::Service *service) : service(service)
    {}

    TimeEvents::~TimeEvents()
    {
        StopTimer();
    }

    std::unique_ptr<sys::Timer> &TimeEvents::Timer()
    {
        if (fireEventTimer == nullptr) {
            fireEventTimer = std::make_unique<sys::Timer>(
                TimerName(), service, eventTimerInitInterval, sys::Timer::Type::SingleShot);
        }
        return fireEventTimer;
    }

    void TimeEvents::StartProcessing()
    {
        timersProcessingStarted = true;
        ProcessNextEvent();
    }

    void TimeEvents::StopProcessing()
    {
        StopTimer();
        timersProcessingStarted = false;
    }

    void TimeEvents::ProcessNextEvent()
    {
        StopTimer();
        if (!IsStarted()) {
            return;
        }

        SendNextEventQuery();
    }

    void TimeEvents::StopTimer()
    {
        Timer()->stop();
    }

    void TimeEvents::RecreateTimer(uint32_t interval)
    {
        StopTimer();
        if (!IsStarted()) {
            return;
        }

        Timer()->connect([=](sys::Timer &) { fireEventTimerCallback(); });
        Timer()->reload(interval);
    }

    void TimeEvents::fireEventTimerCallback()
    {
        if (!IsStarted()) {
            return;
        }

        InvokeEvent();
        SendEventFiredQuery();
    }

    bool TimeEvents::ReceiveNextEventQuery(std::unique_ptr<db::QueryResult> nextEventQueryResult)
    {
        if (!IsStarted()) {
            return true;
        }
        if (nextEventQueryResult == nullptr) {
            return false;
        }

        uint32_t interval = CalcToNextEventInterval(std::move(nextEventQueryResult));
        if (interval > 0) {
            RecreateTimer(interval);
        }
        return true;
    }
} // namespace stm

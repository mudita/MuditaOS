// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Timers/SystemTimer.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerMessage.hpp>
#include <log/log.hpp>
#include <projdefs.h>
#include <memory>

#if DEBUG_TIMER == 1
#define log_debug(...) LOG_DEBUG(__VA_ARGS__)
#else
#define log_debug(...)
#endif

namespace sys::timer
{
    SystemTimer::SystemTimer(Service *parent,
                             const std::string &name,
                             std::chrono::milliseconds interval,
                             timer::Type type)
        : cpp_freertos::Timer(name.c_str(), pdMS_TO_TICKS(interval.count()), type == timer::Type::Periodic), name{name},
          interval{interval}, type{type}, parent{parent}
    {
        attachToService();
        log_debug("%s %s timer created", name.c_str(), type == Type::Periodic ? "periodic" : "single-shot");
    }

    void SystemTimer::attachToService()
    {
        assert(parent != nullptr);
        parent->getTimers().attach(this);
    }

    SystemTimer::~SystemTimer() noexcept
    {
        parent->getTimers().detach(this);
    }

    void SystemTimer::Run()
    {
        auto msg = std::make_shared<TimerMessage>(this);
        if (const auto ret = parent->bus.sendUnicast(std::move(msg), parent->GetName()); !ret) {
            LOG_ERROR("Timer %s error: bus error", name.c_str());
        }
    }

    void SystemTimer::start()
    {
        log_debug("Timer %s start", name.c_str());
        stopTimerIfActive();
        startTimer();
    }

    void SystemTimer::restart(std::chrono::milliseconds newInterval)
    {
        log_debug("Timer %s restart", name.c_str());
        stopTimerIfActive();
        setInterval(newInterval);
        startTimer();
    }

    void SystemTimer::stopTimerIfActive()
    {
        if (active) {
            cpp_freertos::Timer::Stop(0);
        }
    }

    void SystemTimer::startTimer()
    {
        active = true;
        cpp_freertos::Timer::Start(0);
    }

    void SystemTimer::stop()
    {
        log_debug("Timer %s stop!", name.c_str());
        // make sure callback is not called even if it is already in the queue
        active = false;
        cpp_freertos::Timer::Stop(0);
    }

    void SystemTimer::setInterval(std::chrono::milliseconds value)
    {
        log_debug("Timer %s set interval to %ld ms!", name.c_str(), static_cast<long int>(value.count()));
        interval = value;
        cpp_freertos::Timer::SetPeriod(pdMS_TO_TICKS(interval.count()), 0);
    }

    void SystemTimer::onTimeout()
    {
        log_debug("Timer %s tick", name.c_str());
        if (!callback) {
            LOG_WARN("Timer %s error: Callback is not valid.", name.c_str());
            return;
        }
        if (!isActive()) {
            LOG_WARN("Timer %s error: Timer is not active.", name.c_str());
            return;
        }
        log_debug("Timer %s runs callback", name.c_str());
        if (type == timer::Type::SingleShot) {
            stop();
        }
        callback(*this);
    }

    bool SystemTimer::isActive() const noexcept
    {
        return active;
    }

    void SystemTimer::connect(timer::TimerCallback &&newCallback) noexcept
    {
        callback = std::move(newCallback);
    }
} // namespace sys::timer

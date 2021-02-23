// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Timer.hpp"
#include "Service.hpp"
#include "TimerMessage.hpp"
#include <log/log.hpp>
#include <projdefs.h>
#include <limits>
#include <memory>
#include <cstdint>

#if DEBUG_TIMER == 1
#define log_debug(...) LOG_DEBUG(__VA_ARGS__)
#else
#define log_debug(...)
#endif

#define log_error(...) LOG_ERROR(__VA_ARGS__)
#define log_warn(...)  LOG_WARN(__VA_ARGS__)

namespace sys
{

    const ms Timer::timeout_infinite = std::numeric_limits<ms>().max();
    static uint32_t timer_id;

    auto toName(const std::string &val, uint32_t no) -> std::string
    {
        return val + "_" + std::to_string(no);
    }

    Timer::Timer(const std::string &name, Service *service, ms interval, Type type)
        : cpp_freertos::Timer((toName(name, timer_id)).c_str(), pdMS_TO_TICKS(interval), type == Type::Periodic),
          parent(service), type(type), interval(interval), name(toName(name, timer_id))
    {
        if (service != nullptr) {
            service->getTimers().attach(this);
        }
        else {
            log_error("Bad timer creation!");
        }
        ++timer_id;
        log_debug("Timer %s created %s", name.c_str(), type == Type::Periodic ? "periodic" : "singleshot");
    }

    Timer::Timer(Service *parent, ms interval, Type type) : Timer("Timer", parent, interval, type)
    {}

    Timer::~Timer()
    {
        parent->getTimers().detach(this);
        stop();
    }

    void Timer::Run()
    {
        auto msg = std::make_shared<TimerMessage>(this);
        if (parent == nullptr) {
            log_error("Timer %s error: no parent service", name.c_str());
            return;
        }
        if (!parent->bus.sendUnicast(msg, parent->GetName())) {
            LOG_ERROR("Timer %s error: bus error", name.c_str());
            return;
        }
    }

    void Timer::start()
    {
        log_debug("Timer %s start!", name.c_str());
        isActive = true;
        Start(0);
    }

    void Timer::reload(ms from_time)
    {
        Stop(0); // to avoid retriggering timer
        if (from_time != 0) {
            interval = from_time;
            SetPeriod(pdMS_TO_TICKS(interval));
        }
        log_debug("Timer %s reload!", name.c_str());
        isActive = true;
        Start(0); // start with no waittime
    }

    void Timer::stop()
    {
        log_debug("Timer %s stop!", name.c_str());
        // make sure callback is not called even if it is already in the queue
        isActive = false;
        Stop(0);
    }

    void Timer::setInterval(ms new_interval)
    {
        log_debug("Timer %s set interval to: %d ms!", name.c_str(), static_cast<unsigned int>(interval));
        interval = new_interval;
        SetPeriod(pdMS_TO_TICKS(new_interval), 0);
    }

    void Timer::onTimeout()
    {
        log_debug("Timer %s tick", name.c_str());
        if (callback != nullptr && isActive) {
            log_debug("Timer %s callback run!", name.c_str());
            callback(*this);
            return;
        }
        log_warn(
            "callback from %s non valid - %d, or not active - %d", name.c_str(), callback == nullptr, isActive != true);
    }
} // namespace sys

// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "portmacro.h"
#include <Timers/Timer.hpp>
#include <timer.hpp>
#include <functional>
#include <string>
#include <atomic>

namespace sys
{
    class Service;
} // namespace sys

namespace sys::timer
{
    class SystemTimer : public Timer, private cpp_freertos::Timer
    {
      public:
        /// Create named timer and register it in parent
        /// @param parent service on which behalf timer events will be sent and received
        /// @param name this will be name of timer + postfix
        /// @param interval time for next timer event in
        /// @param type type of timer
        SystemTimer(Service *parent, const std::string &name, std::chrono::milliseconds interval, Type type);
        SystemTimer(const SystemTimer &)     = delete;
        SystemTimer(SystemTimer &&) noexcept = delete;
        SystemTimer &operator=(const SystemTimer &) = delete;
        SystemTimer &operator=(SystemTimer &&) noexcept = delete;
        ~SystemTimer() noexcept override;

        void start() override;
        void restart(std::chrono::milliseconds newInterval) override;
        void stop() override;
        [[nodiscard]] bool isActive() const noexcept override;

        void setInterval(std::chrono::milliseconds value);
        void connect(TimerCallback &&newCallback) noexcept;
        void onTimeout();

      private:
        /// This is final by design - to avoid missuse we send Timer notification to Service
        /// and then handle it like any other event. Not by callback as this could cause unrestricted access (no mutex)
        void Run() final;
        /// executed when we receive timeout notification (from overriden Run()) on bus
        void startTimer();
        void stopTimerIfActive();
        void attachToService();

        std::string name;
        TimerCallback callback;
        std::chrono::milliseconds interval;
        Type type;
        Service *parent         = nullptr;
        std::atomic_bool active = false;
    };
}; // namespace sys::timer

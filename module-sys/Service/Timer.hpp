// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "portmacro.h"                             // for TickType_t
#include <module-os/RTOSWrapper/include/timer.hpp> // for Timer
#include <functional>                              // for function
#include <string>                                  // for string
#include <atomic>
namespace sys
{
    class Service;
} // namespace sys

namespace sys
{
    using ms = unsigned int;

    /// Base timer for all coarse timers in system
    class Timer : public cpp_freertos::Timer
    {
      public:
        static const ms timeout_infinite;

        enum class Type
        {
            Periodic,
            SingleShot
        };

        /// Create named timer and register it in parent
        /// @param name this will be name of timer + postfix
        /// @param parent service on which behalf timer events will be sent and received
        /// @param interval time for next timer event in
        Timer(const std::string &name, Service *parent, ms interval, Type type = Type::Periodic);

        /// Create timer with default name `_<id_number>` and register it in parent
        /// @param parent service on which behalf timer events will be sent and received
        /// @param interval time for next timer event in
        Timer(Service *parent, ms interval, Type type = Type::Periodic);

        Timer()              = delete;
        Timer(const Timer &) = delete;
        Timer operator=(const Timer &) = delete;

        /// just to stop timer
        ~Timer() override;

        /// This is final by design - to avoid missuse we send Timer notification to Service
        /// and then handle it like any other event. Not by callback as this could cause unrestricted access (no mutex)
        void Run() final;

        /// executed when we receive timeout notification (from overriden Run()) on bus
        void onTimeout();

        /// @defgroup API
        /// {
        void start();
        void reload(ms from_time = 0);
        void stop();
        void setInterval(ms new_interval);
        /// }

      protected:
        Service *parent = nullptr;

      private:
        Type type;
        ms interval;
        std::function<void(Timer &)> callback = nullptr;
        std::atomic_bool isActive             = false;
        const std::string name;

      public:
        void connect(std::function<void(Timer &)> new_callback)
        {
            callback = new_callback;
        }

        [[nodiscard]] constexpr auto getName() const -> const std::string &
        {
            return name;
        }
    };
}; // namespace sys

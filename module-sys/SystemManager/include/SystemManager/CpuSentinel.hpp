// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <bsp/common.hpp>

#include <string>
#include <functional>
#include <atomic>
#include <Timers/TimerHandle.hpp>

namespace sys
{
    /// Each sentinel manages the requests, i.e. when it is needed it sends messages to CpuGovernor with the required
    /// minimum CPU frequency to perform the task (e.g. screen redraw). Furthermore, every sentinel is informed
    /// immediately after changing the frequency. This allows it to invoke a callback to the service to update their
    /// resources (e.g. PWM filling). Every sentinel must register itself on startup to CpuGovernor by sending
    /// "SentinelRegistrationMessage".
    class CpuSentinel
    {
      public:
        explicit CpuSentinel(std::string name,
                             sys::Service *service,
                             std::function<void(bsp::CpuFrequencyMHz)> callback = nullptr);

        [[nodiscard]] auto GetName() const noexcept -> std::string;
        void HoldMinimumFrequency(bsp::CpuFrequencyMHz frequencyToHold);
        void ReleaseMinimumFrequency();

        [[nodiscard]] auto GetFrequency() const noexcept -> bsp::CpuFrequencyMHz;

        void CpuFrequencyHasChanged(bsp::CpuFrequencyMHz newFrequency);

        void ReadRegistrationData(bsp::CpuFrequencyMHz frequencyHz);
        TaskHandle_t getTask();
        std::string getReason();

      protected:
        const std::string name;
        bsp::CpuFrequencyMHz currentFrequencyToHold{bsp::CpuFrequencyMHz::Level_0};
        std::atomic<bsp::CpuFrequencyMHz> currentFrequency{bsp::CpuFrequencyMHz::Level_0};
        sys::Service *owner{nullptr};

        /// function called from the PowerManager context
        /// to update resources immediately
        /// critical section or mutex support necessary
        std::function<void(bsp::CpuFrequencyMHz)> callback;

        std::string currentReason;
    };

    /// Sentinel releases the frequency lock automatically after the time specified in the parameter - timeout
    class TimedCpuSentinel : public CpuSentinel
    {
      public:
        TimedCpuSentinel(std::string name, sys::Service *service);
        ~TimedCpuSentinel();
        void HoldMinimumFrequencyForTime(bsp::CpuFrequencyMHz frequencyToHold, std::chrono::milliseconds timeout);

      private:
        sys::TimerHandle timerHandle;
    };

} // namespace sys

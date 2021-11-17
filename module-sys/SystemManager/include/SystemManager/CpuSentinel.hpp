// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <bsp/common.hpp>

#include <string>
#include <functional>
#include <atomic>

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
                             std::function<void(bsp::CpuFrequencyHz)> callback = nullptr);
        ~CpuSentinel() = default;

        [[nodiscard]] auto GetName() const noexcept -> std::string;
        void HoldMinimumFrequency(bsp::CpuFrequencyHz frequencyToHold);
        bool HoldMinimumFrequencyAndWait(bsp::CpuFrequencyHz frequencyToHold,
                                         TaskHandle_t taskToNotify,
                                         uint32_t timeout);
        void ReleaseMinimumFrequency();
        void CpuFrequencyHasChanged(bsp::CpuFrequencyHz newFrequency);

      protected:
        const std::string name;
        bsp::CpuFrequencyHz currentFrequencyToHold{bsp::CpuFrequencyHz::Level_0};
        std::atomic<bsp::CpuFrequencyHz> currentFrequency{bsp::CpuFrequencyHz::Level_0};
        sys::Service *owner{nullptr};

        /// function called from the PowerManager context
        /// to update resources immediately
        /// critical section or mutex support necessary
        std::function<void(bsp::CpuFrequencyHz)> callback;

        TaskHandle_t taskHandle = nullptr;
    };

} // namespace sys

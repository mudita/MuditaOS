// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <mutex.hpp>
#include <bsp/common.hpp>
#include "SentinelView.hpp"

namespace sys
{
    class LogSentinel
    {
      public:
        explicit LogSentinel(bsp::CpuFrequencyMHz frequencyToLock);

        void HoldMinimumFrequency();
        void ReleaseMinimumFrequency();
        void UpdateCurrentFrequency(bsp::CpuFrequencyMHz newFrequency);
        [[nodiscard]] auto GetRequestedFrequency() const noexcept -> sentinel::View;

      private:
        xSemaphoreHandle mutex;
        bsp::CpuFrequencyMHz requestedFrequency{bsp::CpuFrequencyMHz::Level_0};
        bool dumpPermission{true};
        bool frequencyRequest{false};
    };

} // namespace sys

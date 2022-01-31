// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/cmd/CSQ.hpp>

#include <cstdint>
#include <chrono>
#include <functional>

namespace cellular::service
{

    constexpr auto urcThreshold = 4;
    constexpr auto pollTime     = std::chrono::minutes{15};

    static const auto invalid_rssi_low  = 99;
    static const auto invalid_rssi_high = 199;

    enum class CSQMode
    {
        Reporting,
        Polling
    };

    class CSQHandler
    {
      public:
        void handleTimerTick();
        void handleURCCounterMessage(const uint32_t counter);

        std::function<bool()> onEnableCsqReporting;
        std::function<bool()> onDisableCsqReporting;
        std::function<std::optional<at::result::CSQ>()> onGetCsq;
        std::function<void(uint32_t)> onPropagateCSQ;
        std::function<void()> onInvalidCSQ;

        std::function<void(bool)> onRetrySwitchMode;
        std::function<void()> onRetryGetCSQ;

        bool switchToReportMode();
        bool switchToPollMode();
        bool getCSQ();

      private:
        uint32_t urcCounter = 0;
        CSQMode currentMode = CSQMode::Reporting;
        std::chrono::minutes timeSpentInPollMode{};

        auto isInPollMode() -> bool;
        auto isPollModeTimeElapsed() -> bool;
        auto isTooManyURC() -> bool;
    };
} // namespace cellular::service

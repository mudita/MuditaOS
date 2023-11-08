// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/cmd/CSQ.hpp>

#include <cstdint>
#include <chrono>
#include <functional>
#include <FreeRTOS.h>

namespace cellular::service
{
    enum class CSQMode
    {
        PermanentReporting,
        HybridReporting,
        HybridPolling
    };

    class CSQHandler
    {
      public:
        void handleTimerTick();
        void handleURCCounterMessage(std::uint32_t counter);

        std::function<bool()> onEnableCsqReporting;
        std::function<bool()> onDisableCsqReporting;
        std::function<std::optional<at::result::CSQ>()> onGetCsq;
        std::function<void(std::uint32_t)> onPropagateCSQ;
        std::function<void()> onInvalidCSQ;

        std::function<void(CSQMode)> onRetrySwitchMode;
        std::function<void()> onRetryGetCSQ;

        void handleLockPhone();
        void handleUnlockPhone();
        void handleBluetoothCarKitConnect();
        void handleBluetoothCarKitDisconnect();
        void checkConditionToChangeMode();
        bool switchToPermanentReportMode();
        bool switchToHybridReportMode();
        bool switchToHybridPollMode();
        bool getCSQ();

      private:
        std::uint32_t urcCounter = 0;
        CSQMode currentMode = CSQMode::HybridReporting;
        TickType_t switchToPollModeTimestamp{0};

        // parameters needed to switch poll/report mode
        bool isPhoneLocked{true};
        bool isBluetoothCarKitConnected{false};

        auto isPollModeTimeElapsed() -> bool;
        auto isTooManyURCs() -> bool;
    };
} // namespace cellular::service

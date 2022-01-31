// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CSQHandler.hpp"

#include <log/log.hpp>

namespace cellular::service
{

    void CSQHandler::handleTimerTick()
    {
        if (isInPollMode()) {
            timeSpentInPollMode++;
            if (isPollModeTimeElapsed()) {
                switchToReportMode();
                return;
            }

            getCSQ();
        }
    }

    void CSQHandler::handleURCCounterMessage(const uint32_t counter)
    {
        urcCounter = counter;
        if (isTooManyURC() && not isInPollMode()) {
            switchToPollMode();
        }
    }

    auto CSQHandler::isInPollMode() -> bool
    {
        return currentMode == CSQMode::Polling;
    }

    auto CSQHandler::isTooManyURC() -> bool
    {
        return urcCounter > urcThreshold;
    }

    auto CSQHandler::isPollModeTimeElapsed() -> bool
    {
        return timeSpentInPollMode > pollTime;
    }

    bool CSQHandler::switchToReportMode()
    {
        LOG_INFO("CSQ poll mode time elapsed, switch to report mode.");
        if (onEnableCsqReporting != nullptr && onEnableCsqReporting()) {
            timeSpentInPollMode = std::chrono::minutes{0};
            currentMode         = CSQMode::Reporting;
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ report mode! Retry!");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(false);
        }
        return false;
    }

    bool CSQHandler::switchToPollMode()
    {
        LOG_INFO("Too many signal strength updates, switch to poll mode.");
        if (onDisableCsqReporting != nullptr && onDisableCsqReporting()) {
            currentMode = CSQMode::Polling;
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ poll mode! Retry!");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(false);
        }
        return false;
    }

    bool CSQHandler::getCSQ()
    {
        if (onGetCsq != nullptr) {
            if (auto result = onGetCsq(); result.has_value()) {
                auto csq = result.value();
                if (csq.csq != invalid_rssi_low && csq.csq != invalid_rssi_high) {
                    LOG_INFO("Propagate valid CSQ");
                    if (onPropagateCSQ != nullptr) {
                        onPropagateCSQ(csq.csq);
                        return true;
                    }
                }
                else {
                    LOG_INFO("Invalid CSQ, notify service antenna");
                    if (onInvalidCSQ != nullptr) {
                        onInvalidCSQ();
                        return true;
                    }
                }
            }
        }

        LOG_ERROR("Failed to get CSQ! Retry!");
        if (onRetryGetCSQ != nullptr) {
            onRetryGetCSQ();
        }
        return false;
    }
} // namespace cellular::service

// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CSQHandler.hpp"

#include <log/log.hpp>
#include <EventStore.hpp>
#include <ticks.hpp>
#include <chrono>

namespace cellular::service
{

    void CSQHandler::handleTimerTick()
    {
        if (currentMode == CSQMode::HybridPolling) {
            if (isPollModeTimeElapsed()) {
                LOG_INFO("CSQ poll mode time elapsed.");
                switchToHybridReportMode();
                return;
            }

            getCSQ();
        }
    }

    void CSQHandler::handleURCCounterMessage(const uint32_t counter)
    {
        urcCounter = counter;
        if (isTooManyURC() && currentMode == CSQMode::HybridReporting) {
            switchToHybridPollMode();
        }
    }

    auto CSQHandler::isTooManyURC() -> bool
    {
        return urcCounter > urcThreshold;
    }

    auto CSQHandler::isPollModeTimeElapsed() -> bool
    {
        auto currentTime = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
        auto timeSpentInPollMode =
            currentTime >= switchToPollModeTimestamp
                ? currentTime - switchToPollModeTimestamp
                : std::numeric_limits<TickType_t>::max() - switchToPollModeTimestamp + currentTime;
        return timeSpentInPollMode > std::chrono::duration_cast<std::chrono::milliseconds>(pollTime).count();
    }

    void CSQHandler::checkConditionToChangeMode()
    {
        if (currentMode != CSQMode::PermanentReporting) {
            if (not isPhoneLocked || isBluetoothCarKitConnected ||
                Store::Battery::get().state != Store::Battery::State::Discharging) {
                switchToPermanentReportMode();
            }
        }
        else {
            if (isPhoneLocked && not isBluetoothCarKitConnected &&
                Store::Battery::get().state == Store::Battery::State::Discharging) {
                switchToHybridReportMode();
            }
        }
    }

    bool CSQHandler::switchToPermanentReportMode()
    {
        LOG_INFO("Switch to permanent report mode.");
        if (onEnableCsqReporting != nullptr && onEnableCsqReporting()) {
            currentMode = CSQMode::PermanentReporting;
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ permanent report mode! Retry!");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(CSQMode::PermanentReporting);
        }
        return false;
    }

    bool CSQHandler::switchToHybridReportMode()
    {
        LOG_INFO("Switch to hybrid report mode.");
        if (onEnableCsqReporting != nullptr && onEnableCsqReporting()) {
            currentMode = CSQMode::HybridReporting;
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ hybrid report mode! Retry!");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(CSQMode::HybridReporting);
        }
        return false;
    }

    bool CSQHandler::switchToHybridPollMode()
    {
        LOG_INFO("Too many signal strength updates, switch to hybrid poll mode.");
        if (onDisableCsqReporting != nullptr && onDisableCsqReporting()) {
            currentMode               = CSQMode::HybridPolling;
            switchToPollModeTimestamp = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ hybrid poll mode! Retry!");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(CSQMode::HybridPolling);
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

    void CSQHandler::handleLockPhone()
    {
        isPhoneLocked = true;
    }

    void CSQHandler::handleUnlockPhone()
    {
        isPhoneLocked = false;
    }

    void CSQHandler::handleBluetoothCarKitConnect()
    {
        isBluetoothCarKitConnected = true;
    }

    void CSQHandler::handleBluetoothCarKitDisconnect()
    {
        isBluetoothCarKitConnected = false;
    }

} // namespace cellular::service

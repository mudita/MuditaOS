// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CSQHandler.hpp"

#include <log/log.hpp>
#include <EventStore.hpp>
#include <ticks.hpp>
#include <chrono>

namespace
{
    constexpr auto urcThreshold     = 4;
    constexpr auto pollModeDuration = std::chrono::minutes{60};

    bool isRssiValid(std::uint32_t csq)
    {
        constexpr auto invalidRssiLow  = 99;
        constexpr auto invalidRssiHigh = 199;
        return ((csq != invalidRssiLow) && (csq != invalidRssiHigh));
    }
} // namespace

namespace cellular::service
{
    void CSQHandler::handleTimerTick()
    {
        if (currentMode == CSQMode::HybridPolling) {
            if (isPollModeTimeElapsed()) {
                LOG_INFO("CSQ poll mode timer elapsed");
                switchToHybridReportMode();
                return;
            }
            getCSQ();
        }
    }

    void CSQHandler::handleURCCounterMessage(std::uint32_t counter)
    {
        urcCounter = counter;
        if (isTooManyURCs() && (currentMode == CSQMode::HybridReporting)) {
            switchToHybridPollMode();
        }
    }

    auto CSQHandler::isTooManyURCs() -> bool
    {
        return (urcCounter > urcThreshold);
    }

    auto CSQHandler::isPollModeTimeElapsed() -> bool
    {
        const auto currentTime         = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
        const auto timeSpentInPollMode = utils::computeIncrease(currentTime, switchToPollModeTimestamp);
        return (timeSpentInPollMode >= std::chrono::duration_cast<std::chrono::milliseconds>(pollModeDuration).count());
    }

    void CSQHandler::checkConditionToChangeMode()
    {
        if (currentMode != CSQMode::PermanentReporting) {
            if (!isPhoneLocked || isBluetoothCarKitConnected ||
                (Store::Battery::get().state != Store::Battery::State::Discharging)) {
                switchToPermanentReportMode();
            }
        }
        else {
            if (isPhoneLocked && !isBluetoothCarKitConnected &&
                (Store::Battery::get().state == Store::Battery::State::Discharging)) {
                switchToHybridReportMode();
            }
        }
    }

    bool CSQHandler::switchToPermanentReportMode()
    {
        if ((onEnableCsqReporting != nullptr) && onEnableCsqReporting()) {
            currentMode = CSQMode::PermanentReporting;
            LOG_INFO("Switch to permanent report mode");
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ permanent report mode, retrying");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(CSQMode::PermanentReporting);
        }
        return false;
    }

    bool CSQHandler::switchToHybridReportMode()
    {
        if ((onEnableCsqReporting != nullptr) && onEnableCsqReporting()) {
            currentMode = CSQMode::HybridReporting;
            LOG_INFO("Switching to hybrid report mode");
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ hybrid report mode, retrying");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(CSQMode::HybridReporting);
        }
        return false;
    }

    bool CSQHandler::switchToHybridPollMode()
    {
        if ((onDisableCsqReporting != nullptr) && onDisableCsqReporting()) {
            currentMode               = CSQMode::HybridPolling;
            switchToPollModeTimestamp = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
            LOG_INFO("Too many signal strength updates, switching to hybrid poll mode");
            return true;
        }

        LOG_ERROR("Failed to switch to CSQ hybrid poll mode, retrying");
        if (onRetrySwitchMode != nullptr) {
            onRetrySwitchMode(CSQMode::HybridPolling);
        }
        return false;
    }

    bool CSQHandler::getCSQ()
    {
        if (onGetCsq != nullptr) {
            if (const auto &result = onGetCsq(); result.has_value()) {
                const auto &csq = result.value();
                if (isRssiValid(csq.csq)) {
                    LOG_INFO("Propagating valid CSQ");
                    if (onPropagateCSQ != nullptr) {
                        onPropagateCSQ(csq.csq);
                        return true;
                    }
                }
                else {
                    LOG_INFO("Invalid CSQ, notifying service antenna");
                    if (onInvalidCSQ != nullptr) {
                        onInvalidCSQ();
                        return true;
                    }
                }
            }
        }

        LOG_ERROR("Failed to get CSQ, retrying");
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

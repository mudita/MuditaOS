// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/LogSentinel.hpp>
#include <log/log.hpp>

namespace sys
{
    namespace
    {
        constexpr std::uint32_t holdMutexTimeoutMs{100000};
        constexpr auto sentinelName{"LoggerSentinel"};
        constexpr auto holdFrequencyReason{"LogDump"};
    } // namespace

    LogSentinel::LogSentinel(bsp::CpuFrequencyMHz frequencyToLock) : requestedFrequency(frequencyToLock)
    {
        mutex = xSemaphoreCreateMutex();
        if (mutex == NULL) {
            LOG_ERROR("Failed to create mutex for logger!");
        }
    }

    void LogSentinel::HoldMinimumFrequency()
    {
        frequencyRequest = true;
        if (mutex != NULL) {
            if (xSemaphoreTake(mutex, pdMS_TO_TICKS(holdMutexTimeoutMs)) != pdTRUE) {
                LOG_ERROR("Failed to get mutex for the logger!");
            }
        }
        frequencyRequest = false;
    }

    void LogSentinel::ReleaseMinimumFrequency()
    {
        if (mutex != NULL) {
            xSemaphoreGive(mutex);
        }
    }

    void LogSentinel::UpdateCurrentFrequency(bsp::CpuFrequencyMHz newFrequency)
    {
        const bool newPermission = newFrequency >= requestedFrequency;
        if (mutex != NULL && dumpPermission != newPermission) {
            if (newPermission) {
                xSemaphoreGive(mutex);
            }
            else if (xSemaphoreTake(mutex, 0) != pdTRUE) {
                LOG_ERROR("Failed to get mutex when reducing the CPU frequency!");
                return;
            }
        }
        dumpPermission = newPermission;
    }

    [[nodiscard]] auto LogSentinel::GetRequestedFrequency() const noexcept -> sentinel::View
    {
        sentinel::View view{.ownerTCBNumber = 0,
                            .name           = sentinelName,
                            .minFrequency   = (frequencyRequest ||
                                             ((mutex != NULL) && dumpPermission && (uxSemaphoreGetCount(mutex) == 0)))
                                                  ? requestedFrequency
                                                  : bsp::CpuFrequencyMHz::Level_0,
                            .reason         = holdFrequencyReason};
        return view;
    }

} // namespace sys

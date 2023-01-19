
// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>
#include <vector>

namespace alarms::paths
{
    std::filesystem::path getAlarmDir() noexcept;
    std::filesystem::path getMusicDir() noexcept;
    std::filesystem::path getPreWakeUpChimesDir() noexcept;
    std::filesystem::path getSnoozeChimesDir() noexcept;
    std::filesystem::path getBedtimeReminderChimesDir() noexcept;
    std::filesystem::path getBackgroundSoundsDir() noexcept;
    std::filesystem::path getMeditationSoundsDir() noexcept;

    /// Check if system paths exist. In case of error, returns vector of missing entries.
    std::vector<std::filesystem::path> validate() noexcept;
} // namespace alarms::paths

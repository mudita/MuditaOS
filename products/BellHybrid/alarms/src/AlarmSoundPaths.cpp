// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSoundPaths.hpp"

#include <purefs/filesystem_paths.hpp>

namespace alarms::paths
{
    std::filesystem::path getAlarmDir() noexcept
    {
        return purefs::dir::getCurrentOSPath() / "assets/audio/bell/alarm";
    }

    std::filesystem::path getMusicDir() noexcept
    {
        return purefs::dir::getUserDiskPath() / "music";
    }

    std::filesystem::path getPreWakeUpChimesDir() noexcept
    {
        return purefs::dir::getCurrentOSPath() / "assets/audio/bell/prewakeup";
    }

    std::filesystem::path getSnoozeChimesDir() noexcept
    {
        return purefs::dir::getCurrentOSPath() / "assets/audio/bell/chimes";
    }

    std::filesystem::path getBedtimeReminderChimesDir() noexcept
    {
        return purefs::dir::getCurrentOSPath() / "assets/audio/bell/evening_reminder";
    }
} // namespace alarms::paths

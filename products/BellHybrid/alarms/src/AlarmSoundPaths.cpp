// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSoundPaths.hpp"

#include <purefs/filesystem_paths.hpp>

namespace alarms::paths
{
    std::filesystem::path getMusicDir() noexcept
    {
        return purefs::dir::getUserDiskPath() / "music";
    }

    std::filesystem::path getPreWakeUpChimesDir() noexcept
    {
        return purefs::dir::getCurrentOSPath() / "assets/audio/alarm/prewakeup";
    }
    std::filesystem::path getSnoozeChimesDir() noexcept
    {
        return purefs::dir::getCurrentOSPath() / "assets/audio/alarm/snooze";
    }
} // namespace alarms::paths

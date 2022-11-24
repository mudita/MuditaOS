// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AlarmSoundPaths.hpp>

#include <purefs/filesystem_paths.hpp>

namespace alarms::paths
{
    std::filesystem::path getAlarmDir() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio/alarm";
    }

    std::filesystem::path getMusicDir() noexcept
    {
        return purefs::dir::getUserDiskPath() / "music";
    }

    std::filesystem::path getPreWakeUpChimesDir() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio/prewakeup";
    }

    std::filesystem::path getSnoozeChimesDir() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio/chimes";
    }

    std::filesystem::path getBedtimeReminderChimesDir() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio/evening_reminder";
    }

    std::filesystem::path getBackgroundSoundsDir() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio/bg_sounds";
    }

    std::filesystem::path getMeditationSoundsDir() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio/meditation";
    }

    std::vector<std::filesystem::path> validate() noexcept
    {
        std::vector<std::filesystem::path> ret;
        if (not std::filesystem::exists(getAlarmDir())) {
            ret.push_back(getAlarmDir());
        }
        if (not std::filesystem::exists(getMusicDir())) {
            ret.push_back(getMusicDir());
        }
        if (not std::filesystem::exists(getPreWakeUpChimesDir())) {
            ret.push_back(getPreWakeUpChimesDir());
        }
        if (not std::filesystem::exists(getSnoozeChimesDir())) {
            ret.push_back(getSnoozeChimesDir());
        }
        if (not std::filesystem::exists(getBedtimeReminderChimesDir())) {
            ret.push_back(getBedtimeReminderChimesDir());
        }
        if (not std::filesystem::exists(getBackgroundSoundsDir())) {
            ret.push_back(getBackgroundSoundsDir());
        }
        if (not std::filesystem::exists(getMeditationSoundsDir())) {
            ret.push_back(getMeditationSoundsDir());
        }
        return ret;
    }
} // namespace alarms::paths
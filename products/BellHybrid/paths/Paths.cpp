// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Paths.hpp"
#include <purefs/filesystem_paths.hpp>

std::filesystem::path paths::audio::user() noexcept
{
    return purefs::dir::getUserMediaPath() / "app";
}
std::filesystem::path paths::audio::proprietary() noexcept
{
    return purefs::dir::getAssetsDirPath() / "audio";
}
std::filesystem::path paths::audio::alarm() noexcept
{
    return "alarm";
}
std::filesystem::path paths::audio::preWakeup() noexcept
{
    return "prewakeup";
}
std::filesystem::path paths::audio::snooze() noexcept
{
    return "chimes";
}
std::filesystem::path paths::audio::bedtimeReminder() noexcept
{
    return "evening_reminder";
}
std::filesystem::path paths::audio::relaxation() noexcept
{
    return "relaxation";
}
std::filesystem::path paths::audio::meditation() noexcept
{
    return "meditation";
}

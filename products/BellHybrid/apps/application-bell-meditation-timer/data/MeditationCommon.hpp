// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Paths.hpp>
#include <filesystem>
#include <string>

namespace app::meditation
{
    namespace windows
    {
        inline constexpr auto meditationCountdown  = "MeditationCountdown";
        inline constexpr auto meditationProgress   = "MeditationProgress";
        inline constexpr auto meditationLowBattery = "MeditationLowBatteryWindow";
    }; // namespace windows

    constexpr auto meditationDBRecordName = "MeditationTimer";

    inline std::filesystem::path getMeditationAudioPath()
    {
        return paths::audio::proprietary() / paths::audio::meditation() / "Meditation_Gong.mp3";
    }

} // namespace app::meditation

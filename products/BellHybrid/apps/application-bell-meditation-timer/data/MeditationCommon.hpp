// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Paths.hpp>
#include <filesystem>

namespace app::meditation
{
    namespace windows
    {
        inline constexpr auto meditationCountdown  = "MeditationCountdown";
        inline constexpr auto meditationProgress   = "MeditationProgress";
        inline constexpr auto meditationLowBattery = "MeditationLowBatteryWindow";
    } // namespace windows

    inline constexpr auto meditationDBRecordName = "MeditationTimer";

    inline std::filesystem::path getMeditationGongSoundPath()
    {
        return paths::audio::proprietary() / paths::audio::meditation() / "Meditation_Gong.mp3";
    }

    inline std::filesystem::path getMeditationEndSoundPath()
    {
        return paths::audio::proprietary() / paths::audio::meditation() / "Meditation_End.mp3";
    }
} // namespace app::meditation

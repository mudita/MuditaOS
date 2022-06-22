// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AlarmSoundPaths.hpp>

#include <filesystem>
#include <string>

namespace app::meditation
{
    namespace windows
    {
        static constexpr auto meditationCountdown = "MeditationCountdown";
        static constexpr auto meditationProgress  = "MeditationProgress";
        static constexpr auto sessionEnded        = "MeditationSessionEnded";
    }; // namespace windows

    constexpr auto meditationDBRecordName          = "MeditationTimer";
    constexpr auto meditationCountdownDBRecordName = "start_delay";

    inline std::filesystem::path getMeditationAudioPath()
    {
        return alarms::paths::getMeditationSoundsDir() / "Meditation_Gong.mp3";
    }

} // namespace app::meditation

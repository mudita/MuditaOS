// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Paths.hpp>
#include <filesystem>
#include <string>

namespace app::meditation
{
    namespace windows
    {
        static constexpr auto meditationCountdown = "MeditationCountdown";
        static constexpr auto meditationProgress  = "MeditationProgress";
    }; // namespace windows

    constexpr auto meditationDBRecordName = "MeditationTimer";

    inline std::filesystem::path getMeditationAudioPath()
    {
        return paths::audio::proprietary() / "Meditation_Gong.mp3";
    }

} // namespace app::meditation

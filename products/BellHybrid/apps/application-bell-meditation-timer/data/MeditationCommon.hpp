// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app::meditation
{
    namespace windows
    {
        static constexpr auto meditationProgress = "MeditationProgress";
        static constexpr auto readyGoing         = "MeditationReadyGoing";
        static constexpr auto sessionEnded       = "MeditationSessionEnded";
    }; // namespace windows

    constexpr auto meditationDBRecordName = "MeditationTimer";
    inline constexpr auto meditationAudioPath = "assets/audio/meditation/Meditation_Gong.mp3";
} // namespace app::meditation

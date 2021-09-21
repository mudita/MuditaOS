// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <Audio/decoder/Decoder.hpp>

namespace app::music_player
{

    enum class SongState
    {
        Playing,
        NotPlaying
    };

    struct SongContext
    {
        static constexpr uint32_t StartPos = 0;
        SongState currentSongState = SongState::NotPlaying;
        std::optional<audio::Token> currentFileToken;
        std::string filePath;
        uint32_t currentPos = StartPos;

        void clear();

        bool isPlaying() const;
        bool isPaused() const;
        bool isValid() const;
    };

} // namespace app::music_player

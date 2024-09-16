// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "SongContext.hpp"
#include <optional>

namespace app::music
{
    void SongContext::clear()
    {
        currentSongState = SongState::NotPlaying;
        currentFileToken = std::nullopt;
        filePath         = "";
        currentPos       = 0;
    }

    bool SongContext::isValid() const
    {
        return (currentFileToken && currentFileToken->IsValid() && !filePath.empty());
    }

    bool SongContext::isPlaying() const
    {
        return isValid() && currentSongState == SongState::Playing;
    }

    bool SongContext::isPaused() const
    {
        return isValid() && currentSongState == SongState::NotPlaying;
    }
} // namespace app::music

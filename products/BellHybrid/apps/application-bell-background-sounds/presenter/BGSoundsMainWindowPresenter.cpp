// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindowPresenter.hpp"
#include <application-music-player/models/SongsRepository.hpp>
namespace app::bgSounds
{
    BGSoundsMainWindowPresenter::BGSoundsMainWindowPresenter()
    {}
    void BGSoundsMainWindowPresenter::loadAudioRecords()
    {
        getView()->setSoundsList();
    }
} // namespace app::bgSounds

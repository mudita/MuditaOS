// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindowPresenter.hpp"
#include "models/BGSoundsRepository.hpp"
#include <application-music-player/models/SongsRepository.hpp>

namespace app::bgSounds
{
    BGSoundsMainWindowPresenter::BGSoundsMainWindowPresenter(std::shared_ptr<AbstractSoundsRepository> soundsRepository)
        : soundsRepository{std::move(soundsRepository)}
    {
        this->soundsRepository->scanMusicFilesList();
    }
    void BGSoundsMainWindowPresenter::loadAudioRecords()
    {
        getView()->setSoundsList(soundsRepository->getMusicFilesList());
    }
} // namespace app::bgSounds

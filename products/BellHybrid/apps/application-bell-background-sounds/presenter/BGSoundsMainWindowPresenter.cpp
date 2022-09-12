// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindowPresenter.hpp"
#include "data/BGSoundsAudioData.hpp"
#include "widgets/SoundListItem.hpp"
#include "ApplicationBellBackgroundSounds.hpp"
#include <apps-common/models/SongsRepository.hpp>
#include <common/SoundsProvider.hpp>

namespace app::bgSounds
{
    BGSoundsMainWindowPresenter::BGSoundsMainWindowPresenter(
        app::ApplicationCommon *app, std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository)
        : soundsProvider{std::make_shared<SoundsProvider>(
              app, sound_list_item::build, std::move(soundsRepository), sound_list_item::size())}
    {
        soundsProvider->setOnSoundItemActivatedCallback([app](const auto &sound) {
            auto audioContext = std::make_unique<gui::BGSoundsAudioContext>(sound);
            auto switchData   = std::make_unique<gui::BGSoundsSwitchData>(std::move(audioContext));
            app->switchWindow(gui::window::name::bgSoundsTimerSelect, std::move(switchData));
        });
    }
    std::shared_ptr<gui::ListItemProvider> BGSoundsMainWindowPresenter::getProvider()
    {
        return soundsProvider;
    }
} // namespace app::bgSounds

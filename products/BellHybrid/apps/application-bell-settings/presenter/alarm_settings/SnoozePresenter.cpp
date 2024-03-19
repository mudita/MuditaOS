// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozePresenter.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"

namespace app::bell_settings
{
    SnoozePresenter::SnoozePresenter(std::unique_ptr<SnoozeListItemProvider> &&provider,
                                     std::unique_ptr<AbstractSnoozeSettingsModel> &&snoozeSettingsModel,
                                     AbstractAudioModel &audioModel)
        : provider{std::move(provider)}, snoozeSettingsModel{std::move(snoozeSettingsModel)}, audioModel{audioModel}
    {
        auto playSound = [this](const UTF8 &val) {
            currentSoundPath = val;
            this->audioModel.setVolume(this->provider->getCurrentVolume(), AbstractAudioModel::PlaybackType::Snooze);
            this->audioModel.play(currentSoundPath, AbstractAudioModel::PlaybackType::Snooze, {});
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { stopSound(); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel.setVolume(
                val, AbstractAudioModel::PlaybackType::Snooze, audio::VolumeUpdateType::SkipUpdateDB);
            if (this->audioModel.hasPlaybackFinished()) {
                playSound(currentSoundPath);
            }
        };
    }

    void SnoozePresenter::saveData()
    {
        stopSound();
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    void SnoozePresenter::loadData()
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto SnoozePresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void SnoozePresenter::stopSound()
    {
        audioModel.stopPlayedByThis({});
    }

    void SnoozePresenter::eraseProviderData()
    {
        provider->clearData();
    }

    void SnoozePresenter::exitWithoutSave()
    {
        snoozeSettingsModel->getSnoozeChimeVolume().restoreDefault();
    }
} // namespace app::bell_settings

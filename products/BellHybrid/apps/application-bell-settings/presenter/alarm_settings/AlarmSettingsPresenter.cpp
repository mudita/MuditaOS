// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSettingsPresenter.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"

namespace app::bell_settings
{
    AlarmSettingsPresenter::AlarmSettingsPresenter(std::unique_ptr<AlarmSettingsListItemProvider> &&provider,
                                                   std::unique_ptr<AbstractAlarmSettingsModel> &&settingsModel,
                                                   AbstractAudioModel &audioModel,
                                                   std::unique_ptr<AbstractFrontlightModel> &&frontlight)
        : provider{std::move(provider)}, settingsModel{std::move(settingsModel)}, audioModel{audioModel},
          frontlight{std::move(frontlight)}
    {

        auto playSound = [this](const UTF8 &val) {
            currentSoundPath = val;
            this->audioModel.setVolume(this->provider->getCurrentVolume(), AbstractAudioModel::PlaybackType::Alarm);
            this->audioModel.play(currentSoundPath, AbstractAudioModel::PlaybackType::Alarm, {});
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { stopSound(); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel.setVolume(
                val, AbstractAudioModel::PlaybackType::Alarm, audio::VolumeUpdateType::SkipUpdateDB);
            if (this->audioModel.hasPlaybackFinished()) {
                playSound(currentSoundPath);
            }
        };

        auto setBrightness = [this](const auto &brightness) {
            this->frontlight->setBacklight(BacklightState::On);
            this->frontlight->setBrightness(brightness);
        };

        this->provider->onFrontlightEnter  = setBrightness;
        this->provider->onFrontlightChange = setBrightness;
        this->provider->onFrontlightExit   = [this]() {
            this->frontlight->revertConfig();
            this->frontlight->setBacklight(BacklightState::On);
        };
    }

    auto AlarmSettingsPresenter::saveData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto AlarmSettingsPresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto AlarmSettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void AlarmSettingsPresenter::eraseProviderData()
    {
        provider->clearData();
    }

    void AlarmSettingsPresenter::stopSound()
    {
        this->audioModel.stopPlayedByThis({});
    }

    void AlarmSettingsPresenter::exitWithSave()
    {
        saveData();
        eraseProviderData();
    }

    void AlarmSettingsPresenter::exitWithRollback()
    {
        this->stopSound();
        settingsModel->getAlarmVolume().restoreDefault();
        eraseProviderData();
    }
} // namespace app::bell_settings

// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSettingsPresenter.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"

namespace app::bell_settings
{
    AlarmSettingsPresenter::AlarmSettingsPresenter(std::unique_ptr<AlarmSettingsListItemProvider> &&provider,
                                                   std::unique_ptr<AbstractAlarmSettingsModel> &&settingsModel,
                                                   AbstractAudioModel &audioModel,
                                                   std::unique_ptr<AbstractSoundsRepository> &&soundsRepository,
                                                   std::unique_ptr<AbstractFrontlightModel> &&frontlight)
        : provider{std::move(provider)}, settingsModel{std::move(settingsModel)}, audioModel{audioModel},
          soundsRepository{std::move(soundsRepository)}, frontlight{std::move(frontlight)}
    {

        auto playSound = [this](const UTF8 &val) {
            this->audioModel.play(
                this->soundsRepository->titleToPath(val).value_or(""), AbstractAudioModel::PlaybackType::Alarm, {});
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { stopSound(); };
        this->provider->onVolumeChange = [this](const auto &val) {
            this->audioModel.setVolume(val, AbstractAudioModel::PlaybackType::Alarm, {});
        };

        auto setBrightness = [this](const auto &brightness) {
            this->frontlight->setStatus(true);
            this->frontlight->setBrightness(brightness);
        };

        this->provider->onFrontlightEnter  = setBrightness;
        this->provider->onFrontlightChange = setBrightness;
        this->provider->onFrontlightExit   = [this]() { this->frontlight->revertUnsavedChanges(); };
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

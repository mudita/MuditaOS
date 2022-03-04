// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSettingsPresenter.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"

namespace app::bell_settings
{
    AlarmSettingsPresenter::AlarmSettingsPresenter(std::shared_ptr<AlarmSettingsListItemProvider> provider,
                                                   std::unique_ptr<AbstractAlarmSettingsModel> model,
                                                   AbstractAudioModel &audioModel,
                                                   std::unique_ptr<AbstractSoundsRepository> soundsRepository)
        : provider(provider),
          model(std::move(model)), audioModel{audioModel}, soundsRepository{std::move(soundsRepository)}
    {

        auto playSound = [this](const UTF8 &val) {
            this->audioModel.play(
                this->soundsRepository->titleToPath(val).value_or(""), AbstractAudioModel::PlaybackType::Alarm, {});
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { stopSound(); };
        this->provider->onVolumeChange = [this](const auto &val) {
            this->audioModel.setVolume(val, AbstractAudioModel::PlaybackType::Alarm, {});
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
        model->getAlarmVolume().restoreDefault();
        eraseProviderData();
    }
} // namespace app::bell_settings

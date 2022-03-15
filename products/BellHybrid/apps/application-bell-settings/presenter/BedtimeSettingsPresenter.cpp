// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsPresenter.hpp"

namespace app::bell_settings
{
    SettingsPresenter::SettingsPresenter(std::shared_ptr<BedtimeSettingsListItemProvider> provider,
                                         std::shared_ptr<AbstractBedtimeModel> model,
                                         AbstractAudioModel &audioModel,
                                         std::unique_ptr<AbstractSoundsRepository> soundsRepository)
        : provider(std::move(provider)),
          model(std::move(model)), audioModel{audioModel}, soundsRepository{std::move(soundsRepository)}
    {
        auto playSound = [this](const UTF8 &val) {
            currentSoundPath = val;
            this->audioModel.play(this->soundsRepository->titleToPath(currentSoundPath).value_or(""),
                                  AbstractAudioModel::PlaybackType::Bedtime,
                                  {});
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { this->stopSound(); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel.setVolume(val, AbstractAudioModel::PlaybackType::Bedtime, {});
            if (this->audioModel.hasPlaybackFinished()) {
                playSound(currentSoundPath);
            }
        };
    }

    auto SettingsPresenter::saveData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto SettingsPresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto SettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void SettingsPresenter::eraseProviderData()
    {
        provider->clearData();
    }

    void SettingsPresenter::stopSound()
    {
        this->audioModel.stopPlayedByThis({});
    }
    void SettingsPresenter::exitWithoutSave()
    {
        model->getBedtimeVolume().restoreDefault();
    }
} // namespace app::bell_settings

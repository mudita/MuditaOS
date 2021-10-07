// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozePresenter.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"

namespace app::bell_settings
{

    SnoozePresenter::SnoozePresenter(std::shared_ptr<SnoozeListItemProvider> provider,
                                     std::unique_ptr<AbstractSnoozeSettingsModel> snoozeSettingsModel,
                                     std::unique_ptr<AbstractAudioModel> audioModel,
                                     std::unique_ptr<AbstractSoundsRepository> soundsRepository)
        : provider{provider}, snoozeSettingsModel{std::move(snoozeSettingsModel)}, audioModel{std::move(audioModel)},
          soundsRepository{std::move(soundsRepository)}
    {
        auto playResponseCb = [this](audio::RetCode retCode, audio::Token token) {
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            this->currentToken = token;
        };

        auto playSound = [this, playResponseCb](const UTF8 &val) {
            currentSoundPath = val;
            this->audioModel->play(this->soundsRepository->titleToPath(currentSoundPath).value_or(""),
                                   playResponseCb,
                                   AbstractAudioModel::PlaybackType::Chime);
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel->setVolume(val, AbstractAudioModel::PlaybackType::Chime);
            playSound(currentSoundPath);
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
        audioModel->stop(currentToken, nullptr);
    }
} // namespace app::bell_settings

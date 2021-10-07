// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpPresenter.hpp"
#include "models/alarm_settings/PrewakeUpListItemProvider.hpp"

namespace app::bell_settings
{
    PrewakeUpWindowPresenter::PrewakeUpWindowPresenter(std::shared_ptr<PrewakeUpListItemProvider> provider,
                                                       std::unique_ptr<AbstractPrewakeUpSettingsModel> model,
                                                       std::unique_ptr<AbstractAudioModel> audioModel,
                                                       std::unique_ptr<AbstractSoundsRepository> soundsRepository)
        : provider(std::move(provider)),
          model(std::move(model)), audioModel{std::move(audioModel)}, soundsRepository{std::move(soundsRepository)}
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
                                   AbstractAudioModel::PlaybackType::PreWakeup);
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { this->audioModel->stop(currentToken, nullptr); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel->setVolume(val, AbstractAudioModel::PlaybackType::PreWakeup);
            playSound(currentSoundPath);
        };
    }

    auto PrewakeUpWindowPresenter::saveData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto PrewakeUpWindowPresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto PrewakeUpWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void PrewakeUpWindowPresenter::eraseProviderData()
    {
        provider->clearData();
    }
    void PrewakeUpWindowPresenter::stopSound()
    {
        this->audioModel->stop(currentToken, nullptr);
    }
} // namespace app::bell_settings

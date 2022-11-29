// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpPresenter.hpp"
#include "models/alarm_settings/PrewakeUpListItemProvider.hpp"

namespace app::bell_settings
{
    PrewakeUpWindowPresenter::PrewakeUpWindowPresenter(std::unique_ptr<PrewakeUpListItemProvider> &&provider,
                                                       std::unique_ptr<AbstractPrewakeUpSettingsModel> &&model,
                                                       AbstractAudioModel &audioModel,
                                                       std::unique_ptr<AbstractSoundsRepository> &&soundsRepository,
                                                       std::unique_ptr<AbstractFrontlightModel> &&frontlight)
        : provider{std::move(provider)}, model{std::move(model)}, audioModel{audioModel},
          soundsRepository{std::move(soundsRepository)}, frontlight{std::move(frontlight)}
    {
        auto playSound = [this](const UTF8 &val) {
            currentSoundPath = val;
            this->audioModel.play(this->soundsRepository->titleToPath(currentSoundPath).value_or(""),
                                  AbstractAudioModel::PlaybackType::PreWakeup,
                                  {});
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { this->stopSound(); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel.setVolume(val, AbstractAudioModel::PlaybackType::PreWakeup, {});
            if (this->audioModel.hasPlaybackFinished()) {
                playSound(currentSoundPath);
            }
        };

        auto setBrightness = [this](const auto &brightness) {
            this->frontlight->setStatus(true);
            this->frontlight->setBrightness(brightness);
        };

        this->provider->onFrontlightEnter  = setBrightness;
        this->provider->onFrontlightChange = setBrightness;
        this->provider->onFrontlightExit   = [this]() { this->frontlight->revertUnsavedChanges(); };
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
        this->audioModel.stopPlayedByThis({});
    }
    void PrewakeUpWindowPresenter::exitWithoutSave()
    {
        model->getChimeVolume().restoreDefault();
    }
} // namespace app::bell_settings

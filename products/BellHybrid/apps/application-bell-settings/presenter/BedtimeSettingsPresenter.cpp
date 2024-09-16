// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BedtimeSettingsPresenter.hpp"

namespace app::bell_settings
{
    SettingsPresenter::SettingsPresenter(ApplicationCommon *app,
                                         std::unique_ptr<BedtimeSettingsListItemProvider> &&provider,
                                         std::shared_ptr<AbstractBedtimeModel> model,
                                         AbstractAudioModel &audioModel,
                                         std::unique_ptr<AudioErrorModel> &&audioErrorModel)
        : app{app}, provider{std::move(provider)}, model{std::move(model)}, audioModel{audioModel},
          audioErrorModel{std::move(audioErrorModel)}
    {
        auto playSound = [this](const UTF8 &val) {
            auto onStartCallback = [this, val](audio::RetCode retCode) {
                if (retCode != audio::RetCode::Success) {
                    handleAudioError(val, gui::AudioErrorType::UnsupportedMediaType);
                }
            };

            auto onFinishedCallback = [this, val](AbstractAudioModel::PlaybackFinishStatus status) {
                if (status == AbstractAudioModel::PlaybackFinishStatus::Error) {
                    handleAudioError(val, gui::AudioErrorType::FileDeleted);
                }
            };

            currentSoundPath = val;
            this->audioModel.setVolume(this->provider->getCurrentVolume(), AbstractAudioModel::PlaybackType::Bedtime);
            this->audioModel.setPlaybackFinishedCb(std::move(onFinishedCallback));
            this->audioModel.play(currentSoundPath,
                                  AbstractAudioModel::PlaybackType::Bedtime,
                                  AbstractAudioModel::PlaybackMode::Single,
                                  std::move(onStartCallback));
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneChange = playSound;
        this->provider->onToneProceed = [this](const auto &path) { return validatePath(path); };

        this->provider->onVolumeEnter = [this, playSound](const auto &val) {
            getView()->deepRefresh();
            playSound(val);
        };
        this->provider->onVolumeExit   = [this](const auto &) { this->stopSound(); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel.setVolume(
                val, AbstractAudioModel::PlaybackType::Bedtime, audio::VolumeUpdateType::SkipUpdateDB);
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

    auto SettingsPresenter::stopSound() -> void
    {
        this->audioModel.stopPlayedByThis({});
    }

    auto SettingsPresenter::exitWithoutSave() -> void
    {
        this->stopSound();
        model->getBedtimeVolume().restoreDefault();
    }

    auto SettingsPresenter::handleAudioError(const UTF8 &path, gui::AudioErrorType errorType) -> void
    {
        const bool validPath = !audioErrorModel->isPathOnErrorList(path);
        if (validPath) {
            audioErrorModel->addPathToErrorList(path, errorType);
            showAudioError(errorType);
        }
    }

    auto SettingsPresenter::validatePath(const UTF8 &path) const -> bool
    {
        const auto errorType = audioErrorModel->getErrorType(path);
        if (errorType.has_value()) {
            showAudioError(errorType.value());
            return true;
        }
        return false;
    }

    auto SettingsPresenter::showAudioError(gui::AudioErrorType errorType) const -> void
    {
        switch (errorType) {
        case gui::AudioErrorType::FileDeleted:
            getView()->handleDeletedFile();
            break;
        default:
            getView()->handleError();
            break;
        }
    }
} // namespace app::bell_settings

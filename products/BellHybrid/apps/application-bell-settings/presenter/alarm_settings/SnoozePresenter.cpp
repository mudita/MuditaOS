// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozePresenter.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"

namespace app::bell_settings
{
    SnoozePresenter::SnoozePresenter(ApplicationCommon *app,
                                     std::unique_ptr<SnoozeListItemProvider> &&provider,
                                     std::unique_ptr<AbstractSnoozeSettingsModel> &&snoozeSettingsModel,
                                     AbstractAudioModel &audioModel,
                                     std::unique_ptr<AudioErrorModel> &&audioErrorModel)
        : app{app}, provider{std::move(provider)}, snoozeSettingsModel{std::move(snoozeSettingsModel)},
          audioModel{audioModel}, audioErrorModel{std::move(audioErrorModel)}
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
            this->audioModel.setVolume(this->provider->getCurrentVolume(), AbstractAudioModel::PlaybackType::Snooze);
            this->audioModel.setPlaybackFinishedCb(std::move(onFinishedCallback));
            this->audioModel.play(
                currentSoundPath, AbstractAudioModel::PlaybackType::Snooze, std::move(onStartCallback));
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;
        this->provider->onToneProceed = [this](const auto &path) { return validatePath(path); };

        this->provider->onVolumeEnter = [this, playSound](const auto &val) {
            getView()->deepRefresh();
            playSound(val);
        };
        this->provider->onVolumeExit   = [this](const auto &) { stopSound(); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel.setVolume(
                val, AbstractAudioModel::PlaybackType::Snooze, audio::VolumeUpdateType::SkipUpdateDB);
            if (this->audioModel.hasPlaybackFinished()) {
                playSound(currentSoundPath);
            }
        };
    }

    auto SnoozePresenter::saveData() -> void
    {
        stopSound();
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto SnoozePresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto SnoozePresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    auto SnoozePresenter::stopSound() -> void
    {
        audioModel.stopPlayedByThis({});
    }

    auto SnoozePresenter::eraseProviderData() -> void
    {
        provider->clearData();
    }

    auto SnoozePresenter::exitWithoutSave() -> void
    {
        snoozeSettingsModel->getSnoozeChimeVolume().restoreDefault();
    }

    auto SnoozePresenter::handleAudioError(const UTF8 &path, gui::AudioErrorType errorType) -> void
    {
        const bool validPath = !audioErrorModel->isPathOnErrorList(path);
        if (validPath) {
            audioErrorModel->addPathToErrorList(path, errorType);
            showAudioError(errorType);
        }
    }

    auto SnoozePresenter::validatePath(const UTF8 &path) const -> bool
    {
        const auto errorType = audioErrorModel->getErrorType(path);
        if (errorType.has_value()) {
            showAudioError(errorType.value());
            return true;
        }
        return false;
    }

    auto SnoozePresenter::showAudioError(gui::AudioErrorType errorType) const -> void
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

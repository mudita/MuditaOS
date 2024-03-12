// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsPresenter.hpp"

namespace app::bell_settings
{
    BedTimeSettingsPresenter::BedTimeSettingsPresenter(std::unique_ptr<SettingsSongsModel> songsModel,
                                                       AbstractAudioModel &audioModel)
        : songsModel{std::move(songsModel)}, audioModel{audioModel}
    {}

    void BedTimeSettingsPresenter::createData(SettingsSongsModel::OnActivateCallback activateCallback,
                                              SettingsSongsModel::OnScrollCallback scrollCallback)
    {
        songsModel->createData(activateCallback, scrollCallback);
        updateViewState();
    }

    void BedTimeSettingsPresenter::updateViewState()
    {
        auto view = getView();
        if (view != nullptr) {
            view->updateViewState();
        }
    }

    void BedTimeSettingsPresenter::updateRecordsCount()
    {
        songsModel->updateRecordsCount();
    }

    std::shared_ptr<SettingsSongsModel> BedTimeSettingsPresenter::getSongsModel()
    {
        return songsModel;
    }

    void BedTimeSettingsPresenter::playSong(std::string path)
    {
        // TODO: [CS] volume
        // this->audioModel.setVolume(this->provider->getCurrentVolume(), AbstractAudioModel::PlaybackType::Bedtime);
        audioModel.play(path, AbstractAudioModel::PlaybackType::Bedtime, {});
    }

    void BedTimeSettingsPresenter::stopSong()
    {
        audioModel.stopPlayedByThis({});
    }
} // namespace app::bell_settings

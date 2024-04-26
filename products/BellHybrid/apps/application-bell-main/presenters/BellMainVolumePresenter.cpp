// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellMainVolumePresenter.hpp"

namespace app::home_screen
{
    BellMainVolumePresenter::BellMainVolumePresenter(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {}

    VolumeData BellMainVolumePresenter::getVolumeData()
    {
        return volumeData;
    }

    void BellMainVolumePresenter::setVolume(AbstractAudioModel::Volume volume)
    {
        audioModel.setVolume(volume, AbstractAudioModel::PlaybackType::Alarm, audio::VolumeUpdateType::SkipUpdateDB);
    }

    void BellMainVolumePresenter::saveVolume(AbstractAudioModel::Volume volume)
    {
        audioModel.setVolume(volume, AbstractAudioModel::PlaybackType::Alarm);
    }

    AbstractAudioModel::Volume BellMainVolumePresenter::getVolume()
    {
        return audioModel.getVolume(AbstractAudioModel::PlaybackType::Alarm).value_or(defaultVolume);
    }
} // namespace app::home_screen

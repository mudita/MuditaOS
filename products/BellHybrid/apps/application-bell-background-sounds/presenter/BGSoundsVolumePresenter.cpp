// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumePresenter.hpp"

namespace app::bgSounds
{
    BGSoundsVolumePresenter::BGSoundsVolumePresenter(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {}

    VolumeData BGSoundsVolumePresenter::getVolumeData()
    {
        return volumeData;
    }

    void BGSoundsVolumePresenter::setVolume(AbstractAudioModel::Volume volume)
    {
        audioModel.setVolume(volume, AbstractAudioModel::PlaybackType::Multimedia, {});
    }
    AbstractAudioModel::Volume BGSoundsVolumePresenter::getVolume()
    {
        return audioModel.getVolume(AbstractAudioModel::PlaybackType::Multimedia).value_or(defaultVolume);
    }
} // namespace app::bgSounds

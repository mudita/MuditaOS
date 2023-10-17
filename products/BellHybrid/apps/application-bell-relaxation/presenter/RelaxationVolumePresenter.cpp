// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationVolumePresenter.hpp"

namespace app::relaxation
{
    RelaxationVolumePresenter::RelaxationVolumePresenter(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {}

    VolumeData RelaxationVolumePresenter::getVolumeData()
    {
        return volumeData;
    }

    void RelaxationVolumePresenter::setVolume(AbstractAudioModel::Volume volume)
    {
        audioModel.setVolume(volume, AbstractAudioModel::PlaybackType::Multimedia, {});
    }
    AbstractAudioModel::Volume RelaxationVolumePresenter::getVolume()
    {
        return audioModel.getVolume(AbstractAudioModel::PlaybackType::Multimedia).value_or(defaultVolume);
    }
} // namespace app::relaxation

// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationVolumePresenter.hpp"

namespace app::relaxation
{
    RelaxationVolumePresenter::RelaxationVolumePresenter(AbstractAudioModel &audioModel, AbstractAlarmModel &alarm)
        : audioModel{audioModel}, alarmModel{alarm}
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

    bool RelaxationVolumePresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUpFrontLight();
            return true;
        }
        return false;
    }
} // namespace app::relaxation

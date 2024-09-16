// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotificationVolumeModel.hpp"

namespace app::focus::models
{
    NotificationVolumeModel::NotificationVolumeModel(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::FocusTimer).value_or(0);
    }

    void NotificationVolumeModel::setValue(std::uint8_t value)
    {
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::FocusTimer);
    }

    std::uint8_t NotificationVolumeModel::getValue() const
    {
        return audioModel.getVolume(AbstractAudioModel::PlaybackType::FocusTimer).value_or(0);
    }

    void NotificationVolumeModel::restoreDefault()
    {
        setValue(defaultValue);
    }
} // namespace app::focus::models

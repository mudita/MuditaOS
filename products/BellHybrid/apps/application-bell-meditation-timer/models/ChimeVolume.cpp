// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ChimeVolume.hpp"

namespace app::meditation::models
{
    ChimeVolume::ChimeVolume(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::Meditation).value_or(0);
    }

    void ChimeVolume::setValue(std::uint8_t value)
    {
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::Meditation);
    }

    std::uint8_t ChimeVolume::getValue() const
    {
        return audioModel.getVolume(AbstractAudioModel::PlaybackType::Meditation).value_or(0);
    }

    void ChimeVolume::restoreDefault()
    {
        setValue(defaultValue);
    }
} // namespace app::meditation::models

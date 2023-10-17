// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChimeVolume.hpp"

namespace app::meditation::models
{

    void ChimeVolume::setValue(std::uint8_t value)
    {
        defaultValue = value;
        audioModel.setVolume(value, AbstractAudioModel::PlaybackType::Meditation, {});
    }
    std::uint8_t ChimeVolume::getValue() const
    {
        return audioModel.getVolume(AbstractAudioModel::PlaybackType::Meditation).value_or(0);
    }

    void ChimeVolume::restoreDefault()
    {
        setValue(defaultValue);
    }

    ChimeVolume::ChimeVolume(AbstractAudioModel &audioModel) : audioModel{audioModel}
    {
        defaultValue = audioModel.getVolume(AbstractAudioModel::PlaybackType::Meditation).value_or(0);
    }
} // namespace app::meditation::models

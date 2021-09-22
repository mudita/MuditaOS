// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumePresenter.hpp"

namespace app::bgSounds
{
    BGSoundsVolumePresenter::BGSoundsVolumePresenter()
    {}

    VolumeData BGSoundsVolumePresenter::getVolumeData()
    {
        return volumeData;
    }

    unsigned int BGSoundsVolumePresenter::getCurrentVolume()
    {
        return currentVolume;
    }

    void BGSoundsVolumePresenter::onVolumeChanged(unsigned int volume)
    {}

} // namespace app::bgSounds

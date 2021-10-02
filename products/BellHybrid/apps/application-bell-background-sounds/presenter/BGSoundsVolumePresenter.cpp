// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumePresenter.hpp"

namespace app::bgSounds
{
    VolumeData BGSoundsVolumePresenter::getVolumeData()
    {
        return volumeData;
    }

    audio::Volume BGSoundsVolumePresenter::getDefaultVolume()
    {
        return audio::defaultVolume;
    }

} // namespace app::bgSounds

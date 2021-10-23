// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumePresenter.hpp"
#include <Application.hpp>

namespace app::bgSounds
{
    BGSoundsVolumePresenter::BGSoundsVolumePresenter(app::ApplicationCommon &app) : app{app}
    {}

    VolumeData BGSoundsVolumePresenter::getVolumeData()
    {
        return volumeData;
    }

    audio::Volume BGSoundsVolumePresenter::getDefaultVolume()
    {
        return audio::defaultVolume;
    }

    void BGSoundsVolumePresenter::increaseVolume()
    {
        app.increaseCurrentVolume();
    }

    void BGSoundsVolumePresenter::decreaseVolume()
    {
        if (getView()->getCurrentVolume() != minVolume) {
            app.decreaseCurrentVolume();
        }
    }
} // namespace app::bgSounds

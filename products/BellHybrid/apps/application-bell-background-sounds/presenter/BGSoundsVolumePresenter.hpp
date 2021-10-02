// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <module-audio/Audio/AudioCommon.hpp>

namespace app::bgSounds
{
    using VolumeData = struct VolumeData
    {
        audio::Volume min;
        audio::Volume max;
        audio::Volume step;
    };

    class BGSoundsVolumeContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<BGSoundsVolumeContract::View>
        {
          public:
            virtual VolumeData getVolumeData()       = 0;
            virtual audio::Volume getDefaultVolume() = 0;
        };
    };

    class BGSoundsVolumePresenter : public BGSoundsVolumeContract::Presenter
    {
        constexpr static struct VolumeData volumeData
        {
            audio::minVolume, audio::maxVolume, audio::defaultVolumeStep
        };

        VolumeData getVolumeData() override;
        audio::Volume getDefaultVolume() override;
    };
} // namespace app::bgSounds

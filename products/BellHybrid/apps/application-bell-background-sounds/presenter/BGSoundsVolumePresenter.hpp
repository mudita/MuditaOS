// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <module-audio/Audio/AudioCommon.hpp>

namespace app
{
    class ApplicationCommon;
}
namespace app::bgSounds
{
    constexpr audio::Volume minVolume = 1u;
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
            virtual audio::Volume getCurrentVolume() const noexcept = 0;
        };
        class Presenter : public BasePresenter<BGSoundsVolumeContract::View>
        {
          public:
            virtual VolumeData getVolumeData()       = 0;
            virtual audio::Volume getDefaultVolume() = 0;
            virtual void increaseVolume()            = 0;
            virtual void decreaseVolume()            = 0;
        };
    };

    class BGSoundsVolumePresenter : public BGSoundsVolumeContract::Presenter
    {
        app::ApplicationCommon &app;
        constexpr static struct VolumeData volumeData
        {
            bgSounds::minVolume, audio::maxVolume, audio::defaultVolumeStep
        };

        VolumeData getVolumeData() override;
        audio::Volume getDefaultVolume() override;
        void increaseVolume() override;
        void decreaseVolume() override;

      public:
        explicit BGSoundsVolumePresenter(app::ApplicationCommon &app);
    };
} // namespace app::bgSounds

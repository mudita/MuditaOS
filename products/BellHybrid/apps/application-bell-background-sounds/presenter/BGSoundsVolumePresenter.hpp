// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app::bgSounds
{
    using VolumeData = struct VolumeData
    {
        unsigned int min;
        unsigned int max;
        unsigned int step;
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
            virtual VolumeData getVolumeData()                = 0;
            virtual unsigned int getCurrentVolume()           = 0;
            virtual void onVolumeChanged(unsigned int volume) = 0;
        };
    };

    class BGSoundsVolumePresenter : public BGSoundsVolumeContract::Presenter
    {
        struct VolumeData volumeData
        {
            0U, 10U, 1U
        };
        unsigned int currentVolume = 5U;

        VolumeData getVolumeData() override;
        unsigned int getCurrentVolume() override;
        void onVolumeChanged(unsigned int volume) override;

      public:
        BGSoundsVolumePresenter();
    };
} // namespace app::bgSounds

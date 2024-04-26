// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractAudioModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::home_screen
{
    using VolumeData = struct VolumeData
    {
        AbstractAudioModel::Volume min;
        AbstractAudioModel::Volume max;
        AbstractAudioModel::Volume step;
    };

    class AbstractBellMainVolumePresenter
    {
      public:
        virtual ~AbstractBellMainVolumePresenter()                 = default;
        virtual VolumeData getVolumeData()                         = 0;
        virtual void setVolume(AbstractAudioModel::Volume volume)  = 0;
        virtual void saveVolume(AbstractAudioModel::Volume volume) = 0;
        virtual AbstractAudioModel::Volume getVolume()             = 0;
    };

    class BellMainVolumePresenter : public AbstractBellMainVolumePresenter
    {
        static constexpr struct VolumeData volumeData
        {
            AbstractAudioModel::minVolume, AbstractAudioModel::maxVolume, 1
        };
        static constexpr AbstractAudioModel::Volume defaultVolume = 5;

        AbstractAudioModel &audioModel;

        VolumeData getVolumeData() override;
        void setVolume(AbstractAudioModel::Volume volume) override;
        void saveVolume(AbstractAudioModel::Volume volume) override;
        AbstractAudioModel::Volume getVolume() override;

      public:
        explicit BellMainVolumePresenter(AbstractAudioModel &audioModel);
    };
} // namespace app::home_screen

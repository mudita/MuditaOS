// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractAudioModel.hpp>

namespace app
{
    class ApplicationCommon;
}
namespace app::bgSounds
{
    using VolumeData = struct VolumeData
    {
        AbstractAudioModel::Volume min;
        AbstractAudioModel::Volume max;
        AbstractAudioModel::Volume step;
    };

    class AbstractBGSoundsVolumePresenter
    {
      public:
        virtual ~AbstractBGSoundsVolumePresenter()                = default;
        virtual VolumeData getVolumeData()                        = 0;
        virtual void setVolume(AbstractAudioModel::Volume volume) = 0;
        virtual AbstractAudioModel::Volume getVolume()            = 0;
    };

    class BGSoundsVolumePresenter : public AbstractBGSoundsVolumePresenter
    {
        constexpr static struct VolumeData volumeData
        {
            AbstractAudioModel::minVolume, AbstractAudioModel::maxVolume, 1
        };
        static constexpr AbstractAudioModel::Volume defaultVolume = 5;

        AbstractAudioModel &audioModel;

        VolumeData getVolumeData() override;
        void setVolume(AbstractAudioModel::Volume volume) override;
        AbstractAudioModel::Volume getVolume() override;

      public:
        explicit BGSoundsVolumePresenter(AbstractAudioModel &audioModel);
    };
} // namespace app::bgSounds

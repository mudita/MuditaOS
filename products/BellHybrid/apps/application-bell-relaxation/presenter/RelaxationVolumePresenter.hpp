// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractAudioModel.hpp>
#include <common/models/AbstractAlarmModel.hpp>

namespace app
{
    class ApplicationCommon;
}
namespace app::relaxation
{
    using VolumeData = struct VolumeData
    {
        AbstractAudioModel::Volume min;
        AbstractAudioModel::Volume max;
        AbstractAudioModel::Volume step;
    };

    class AbstractRelaxationVolumePresenter
    {
      public:
        virtual ~AbstractRelaxationVolumePresenter()              = default;
        virtual VolumeData getVolumeData()                        = 0;
        virtual void setVolume(AbstractAudioModel::Volume volume) = 0;
        virtual AbstractAudioModel::Volume getVolume()            = 0;
        virtual bool handleIfPreWakeupIsToTurnOffFirst()            = 0;
    };

    class RelaxationVolumePresenter : public AbstractRelaxationVolumePresenter
    {
        constexpr static struct VolumeData volumeData
        {
            AbstractAudioModel::minVolume, AbstractAudioModel::maxVolume, 1
        };
        static constexpr AbstractAudioModel::Volume defaultVolume = 5;

        AbstractAudioModel &audioModel;
        AbstractAlarmModel &alarmModel;

        VolumeData getVolumeData() override;
        void setVolume(AbstractAudioModel::Volume volume) override;
        AbstractAudioModel::Volume getVolume() override;
        bool handleIfPreWakeupIsToTurnOffFirst() override;

      public:
        explicit RelaxationVolumePresenter(AbstractAudioModel &audioModel, AbstractAlarmModel &alarm);
    };
} // namespace app::relaxation

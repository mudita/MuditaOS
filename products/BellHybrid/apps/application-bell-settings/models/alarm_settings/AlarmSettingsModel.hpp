// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmSettingsModel.hpp"
#include <common/models/SettingsModel.hpp>
#include <common/models/AudioModel.hpp>

namespace app::bell_settings
{
    class AlarmToneModel : public gui::SettingsModel<UTF8>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(UTF8 value) override;
        UTF8 getValue() const override;
    };

    class AlarmVolumeModel : public gui::AbstractSettingsModel<std::uint8_t>
    {
      public:
        explicit AlarmVolumeModel(AbstractAudioModel &audioModel);
        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
        void restoreDefault() override;

      private:
        AbstractAudioModel &audioModel;
        std::uint8_t defaultValue;
    };

    class AlarmLightOnOffModel : public gui::SettingsModel<bool>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(bool value) override;
        bool getValue() const override;
    };

    class AlarmSettingsModel : public AbstractAlarmSettingsModel
    {
      public:
        AlarmSettingsModel(std::unique_ptr<AlarmToneModel> alarmToneModel,
                           std::unique_ptr<AlarmVolumeModel> alarmVolumeModel,
                           std::unique_ptr<AlarmLightOnOffModel> alarmLightOnOffModel)
            : AbstractAlarmSettingsModel(
                  std::move(alarmToneModel), std::move(alarmVolumeModel), std::move(alarmLightOnOffModel))
        {}
    };
} // namespace app::bell_settings

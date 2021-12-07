// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>

#include <cstdint>
#include <utf8/UTF8.hpp>

namespace app::bell_settings
{
    class AbstractAlarmSettingsModel
    {
      public:
        AbstractAlarmSettingsModel(std::unique_ptr<gui::AbstractSettingsModel<UTF8>> alarmTone,
                                   std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> alarmVolume,
                                   std::unique_ptr<gui::AbstractSettingsModel<bool>> alarmLightOnOff)
            : alarmTone(std::move(alarmTone)), alarmVolume(std::move(alarmVolume)),
              alarmLightOnOff(std::move(alarmLightOnOff))
        {}

        virtual ~AbstractAlarmSettingsModel() = default;

        gui::AbstractSettingsModel<UTF8> &getAlarmTone()
        {
            return *alarmTone;
        }

        gui::AbstractSettingsModel<std::uint8_t> &getAlarmVolume()
        {
            return *alarmVolume;
        }

        gui::AbstractSettingsModel<bool> &getAlarmLightOnOff()
        {
            return *alarmLightOnOff;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<UTF8>> alarmTone;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> alarmVolume;
        std::unique_ptr<gui::AbstractSettingsModel<bool>> alarmLightOnOff;
    };
} // namespace app::bell_settings

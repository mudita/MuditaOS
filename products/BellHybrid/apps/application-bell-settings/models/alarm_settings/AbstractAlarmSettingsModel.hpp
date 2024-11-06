// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <utf8/UTF8.hpp>
#include <cstdint>

namespace app::bell_settings
{
    class AbstractAlarmSettingsModel
    {
      public:
        AbstractAlarmSettingsModel(std::unique_ptr<gui::AbstractSettingsModel<UTF8>> alarmTone,
                                   std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> alarmVolume,
                                   std::unique_ptr<gui::AbstractSettingsModel<bool>> alarmFadeOnOff,
                                   std::unique_ptr<gui::AbstractSettingsModel<bool>> alarmLightOnOff,
                                   std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> alarmFrontlight)
            : alarmTone(std::move(alarmTone)), alarmVolume(std::move(alarmVolume)),
              alarmFadeOnOff(std::move(alarmFadeOnOff)), alarmLightOnOff(std::move(alarmLightOnOff)),
              alarmFrontlight(std::move(alarmFrontlight))
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

        gui::AbstractSettingsModel<bool> &getAlarmFadeOnOff()
        {
            return *alarmFadeOnOff;
        }

        gui::AbstractSettingsModel<bool> &getAlarmLightOnOff()
        {
            return *alarmLightOnOff;
        }

        gui::AbstractSettingsModel<std::uint8_t> &getBrightness()
        {
            return *alarmFrontlight;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<UTF8>> alarmTone;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> alarmVolume;
        std::unique_ptr<gui::AbstractSettingsModel<bool>> alarmFadeOnOff;
        std::unique_ptr<gui::AbstractSettingsModel<bool>> alarmLightOnOff;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> alarmFrontlight;
    };
} // namespace app::bell_settings

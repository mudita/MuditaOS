// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-db/Settings.hpp>

#include <Service/Service.hpp>

namespace alarms
{
    class FrontlightAction : public AbstractAlarmAction
    {
      public:
        enum class Mode
        {
            Manual,
            LinearProgress
        };

        enum class SettingsDependency
        {
            None,
            AlarmClock,
            Prewakeup
        };

        FrontlightAction(sys::Service &service,
                         Mode mode,
                         SettingsDependency settingsDependency = SettingsDependency::None);
        bool execute() override;
        bool turnOff() override;

      private:
        static constexpr std::string_view alarmFrontlightOFF     = "0";
        static constexpr std::string_view prewakeupFrontlightOFF = "0";

        SettingsDependency settingsDependency;
        std::unique_ptr<AbstractAlarmAction> pimpl;
        settings::Settings settings;
    };
} // namespace alarms

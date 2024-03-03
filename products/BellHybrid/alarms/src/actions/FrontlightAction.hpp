// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-db/Settings.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

namespace sys
{
    class Service;
}

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
        sys::Service &service;
        SettingsDependency settingsDependency;
        std::unique_ptr<AbstractAlarmAction> pimpl;
        settings::Settings settings;
    };
} // namespace alarms

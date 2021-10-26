// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmOptionsItem.hpp"

#include <application-alarm-clock/data/AlarmsData.hpp>
#include <application-alarm-clock/presenter/AlarmRRulePresenter.hpp>
#include <ApplicationCommon.hpp>

namespace gui
{
    class AlarmRRuleOptionsItem : public AlarmOptionsItem, public app::alarmClock::AlarmRRuleItem
    {
      private:
        app::ApplicationCommon *app                                  = nullptr;
        std::function<void(const UTF8 &text)> navBarTemporaryMode    = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode         = nullptr;

        using RRule                                             = app::alarmClock::AlarmRRulePresenter::RRuleOptions;
        std::vector<std::pair<RRule, std::string>> rRuleOptions = {
            {RRule::Never, utils::translate("app_alarm_clock_repeat_never")},
            {RRule::Everyday, utils::translate("app_alarm_clock_repeat_everyday")},
            {RRule::Weekdays, utils::translate("app_alarm_clock_repeat_week_days")},
            {RRule::Custom, utils::translate("app_alarm_clock_repeat_custom")}};

        [[nodiscard]] RRule getRRuleOption(const std::string &selectedOption);
        void checkCustomOption(const std::string &selectedOption);
        void printOptions();

      public:
        explicit AlarmRRuleOptionsItem(app::ApplicationCommon *app,
                                       const std::string &description,
                                       std::shared_ptr<app::alarmClock::AlarmRRulePresenter> presenter,
                                       std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                                       std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);
    };
} /* namespace gui */

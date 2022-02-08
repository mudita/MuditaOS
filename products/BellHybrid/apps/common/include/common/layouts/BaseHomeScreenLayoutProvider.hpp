// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>
#include <utf8/UTF8.hpp>
#include <Temperature.hpp>
#include <time/dateCommon.hpp>

#include <chrono>

namespace app::home_screen
{
    enum class ViewState
    {
        Deactivated,
        DeactivatedWait,
        WaitForConfirmation,
        AlarmEdit,
        ActivatedWait,
        Activated,
        AlarmRinging,
        AlarmRingingDeactivatedWait,
        AlarmSnoozedWait,
        AlarmSnoozed
    };
};

namespace gui
{
    class Item;
    class SnoozeTimer;

    class BaseHomeScreenLayoutProvider
    {
      private:
        std::string name;

      public:
        BaseHomeScreenLayoutProvider(std::string name) : name{std::move(name)} {};
        virtual ~BaseHomeScreenLayoutProvider() noexcept = default;

        std::string getName()
        {
            return name;
        }

        virtual void setAlarmTriggered()                                        = 0;
        virtual void setAlarmActive(bool val)                                   = 0;
        virtual void setViewState(app::home_screen::ViewState state)            = 0;
        virtual void setTemperature(utils::temperature::Temperature newTemp)    = 0;
        virtual void setBottomDescription(const UTF8 &desc)                     = 0;
        virtual void removeBottomDescription()                                  = 0;
        virtual void setBatteryLevelState(const Store::Battery &batteryContext) = 0;
        virtual void setTime(std::time_t newTime)                               = 0;
        virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt)         = 0;
        virtual void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)    = 0;
        virtual void setSnoozeFormat(utils::time::Locale::TimeFormat fmt)       = 0;
        virtual void setAlarmEdit(bool val)                                     = 0;
        virtual std::time_t getAlarmTime() const                                = 0;
        virtual void setAlarmTime(std::time_t newTime)                          = 0;
        virtual void setSnoozeTime(std::time_t newTime)                         = 0;
        virtual SnoozeTimer *getSnoozeTimer()                                   = 0;
        virtual Item *getLayout()                                               = 0;
    };
}; // namespace gui

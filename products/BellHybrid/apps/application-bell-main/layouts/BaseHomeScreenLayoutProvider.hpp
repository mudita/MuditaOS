// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-main/presenters/HomeScreenPresenter.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <Rect.hpp>

namespace gui
{
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

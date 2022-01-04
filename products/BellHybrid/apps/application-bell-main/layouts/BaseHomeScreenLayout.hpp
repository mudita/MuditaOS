#pragma once

#include "presenters/HomeScreenPresenter.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <Rect.hpp>

namespace gui
{
    class BaseHomeScreenLayout : public Rect
    {
      private:
        std::string name;

      public:
        BaseHomeScreenLayout(std::string name)
            : Rect{nullptr, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h}, name{std::move(name)} {};
        virtual ~BaseHomeScreenLayout() noexcept = default;

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
    };
}; // namespace gui

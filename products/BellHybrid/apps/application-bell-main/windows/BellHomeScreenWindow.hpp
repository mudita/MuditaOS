// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/TemperatureModel.hpp"
#include "presenters/HomeScreenPresenter.hpp"
#include "widgets/BellBattery.hpp"

#include <apps-common/GuiTimer.hpp>
#include <apps-common/windows/AppWindow.hpp>

namespace style::bell_home_screen
{
    constexpr inline auto temperature_w = 85U;
    constexpr inline auto temperature_h = 102U;
    constexpr inline auto bottom_desc_w = 390U;
    constexpr inline auto bottom_desc_h = 102U;

    enum class ParentType
    {
        SideListView,
        Window
    };

} // namespace style::bell_home_screen

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmSetSpinner;
    class TimeSetFmtSpinner;
    class SnoozeTimer;

    class BellHomeScreenWindow : public AppWindow, public app::home_screen::AbstractView
    {
      public:
        BellHomeScreenWindow(app::ApplicationCommon *app,
                             std::unique_ptr<app::home_screen::AbstractPresenter> presenter);

      private:
        void buildInterface() override;
        bool updateTime() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onDatabaseMessage(sys::Message *msg) override;

        void setAlarmTriggered() override;
        void setAlarmActive(bool val) override;
        void setAlarmEdit(bool val) override;
        void setHeaderViewMode(app::home_screen::HeaderViewMode mode) override;
        std::time_t getAlarmTime() const override;
        void setAlarmTime(std::time_t newTime) override;
        void incAlarmMinute() override;
        void decAlarmMinute() override;

        void setTemperature(utils::temperature::Temperature newTemp) override;
        void setBottomDescription(const UTF8 &desc) override;
        void setBatteryLevelState(const Store::Battery &batteryContext) override;
        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void switchToMenu() override;
        void switchToBatteryStatus() override;

        BellBaseLayout *body{};

        TimeSetFmtSpinner *time{};
        HBox *bottomBox{};
        BellBattery *battery{};
        TextFixedSize *bottomText{};
        AlarmSetSpinner *alarm{};
        SnoozeTimer *snoozeTimer{};

        std::unique_ptr<app::home_screen::AbstractPresenter> presenter;

        static constexpr auto timerName = "HS_timer";
    };
} // namespace gui

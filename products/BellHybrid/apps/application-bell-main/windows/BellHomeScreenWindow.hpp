// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/TemperatureModel.hpp"
#include "presenters/HomeScreenPresenter.hpp"

#include <apps-common/GuiTimer.hpp>
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmSetSpinner;
    class TimeSetFmtSpinner;

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
        void setAlarmSnoozed() override;
        void setAlarmActive(bool val) override;
        void setAlarmEdit(bool val) override;
        void setAlarmVisible(bool val) override;
        void setAlarmTimeVisible(bool val) override;
        std::time_t getAlarmTime() const override;
        void setAlarmTime(std::time_t newTime) override;
        void incAlarmMinute() override;
        void decAlarmMinute() override;

        void setTemperature(utils::temperature::Temperature newTemp) override;
        void setBottomDescription(const UTF8 &desc) override;
        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void switchToMenu() override;

        BellBaseLayout *body{};

        TimeSetFmtSpinner *time{};
        TextFixedSize *bottomText{};
        AlarmSetSpinner *alarm{};

        std::unique_ptr<app::home_screen::AbstractPresenter> presenter;

        static constexpr auto timerName = "HS_timer";
    };
} // namespace gui

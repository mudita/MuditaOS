// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-main/presenters/HomeScreenPresenter.hpp"
#include "models/TemperatureModel.hpp"

#include <apps-common/GuiTimer.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <common/widgets/BellBattery.hpp>

namespace gui
{
    class BellHomeScreenWindow : public AppWindow, public app::home_screen::AbstractView
    {
      public:
        BellHomeScreenWindow(app::ApplicationCommon *app,
                             std::shared_ptr<app::home_screen::AbstractPresenter> presenter);

      private:
        void buildInterface() override;
        void setLayout(LayoutGenerator layoutGenerator) override;
        RefreshModes updateTime() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onDatabaseMessage(sys::Message *msg) override;

        void setViewState(app::home_screen::ViewState state) override;
        std::time_t getAlarmTime() const override;
        void setAlarmTime(std::time_t newTime) override;
        void setSnoozeTime(std::time_t newTime) override;

        void setTemperature(utils::temperature::Temperature newTemp) override;
        void setTextDescription(const UTF8 &desc) override;
        void setBatteryLevelState(const Store::Battery &batteryContext) override;
        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void setSnoozeFormat(utils::time::Locale::TimeFormat fmt) override;
        bool updateBatteryStatus() override;

        std::shared_ptr<app::home_screen::AbstractPresenter> presenter;

        BaseHomeScreenLayoutProvider *currentLayout{};

        static constexpr auto timerName = "HS_timer";
    };
} // namespace gui

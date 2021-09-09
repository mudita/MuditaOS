// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/presenter/CustomRepeatWindowPresenter.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "Application.hpp"
#include <InputEvent.hpp>
#include <ListView.hpp>

namespace app::alarmClock
{
    class CustomRepeatWindow : public gui::AppWindow, public CustomRepeatWindowContract::View
    {
        gui::ListView *list = nullptr;
        std::unique_ptr<CustomRepeatWindowContract::Presenter> presenter;
        WeekDaysRepeatData weekDaysOptData;

      public:
        CustomRepeatWindow(app::ApplicationCommon *app,
                           std::unique_ptr<CustomRepeatWindowContract::Presenter> &&windowPresenter);

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;
    };

} // namespace app::alarmClock

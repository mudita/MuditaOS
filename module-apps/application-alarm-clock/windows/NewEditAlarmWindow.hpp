// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/models/NewEditAlarmModel.hpp"
#include "application-alarm-clock/presenter/AlarmClockEditWindowPresenter.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <InputEvent.hpp>
#include <ListView.hpp>
#include <Utils.hpp>

namespace app::alarmClock
{
    class NewEditAlarmWindow : public gui::AppWindow, public AlarmClockEditWindowContract::View
    {
        std::shared_ptr<AlarmEventRecord> alarmRecord;
        gui::ListView *list     = nullptr;
        AlarmAction alarmAction = AlarmAction::Add;
        std::unique_ptr<AlarmClockEditWindowContract::Presenter> presenter;

      public:
        NewEditAlarmWindow(app::Application *app,
                           std::unique_ptr<AlarmClockEditWindowContract::Presenter> &&windowPresenter);

        bool handleSwitchData(gui::SwitchData *data) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;
    };

} // namespace app::alarmClock

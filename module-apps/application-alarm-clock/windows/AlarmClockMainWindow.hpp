// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/models/AlarmsModel.hpp"
#include "application-alarm-clock/presenter/AlarmClockMainWindowPresenter.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>
#include <Icon.hpp>

namespace app::alarmClock
{
    class AlarmClockMainWindow : public gui::AppWindow, public AlarmClockMainWindowContract::View
    {
        gui::Icon *emptyListIcon   = nullptr;
        gui::ListView *alarmsList  = nullptr;
        std::unique_ptr<AlarmClockMainWindowContract::Presenter> presenter;
        void showEmptyIcon();
        void showList();
        void onEmptyList();
        void onListFilled();

      public:
        AlarmClockMainWindow(app::Application *app,
                             std::unique_ptr<AlarmClockMainWindowContract::Presenter> &&windowPresenter);
        ~AlarmClockMainWindow() noexcept override;

        bool onDatabaseMessage(sys::Message *msgl) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} // namespace app::alarmClock

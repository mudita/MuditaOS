// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditAlarmWindow.hpp"

namespace app::alarmClock
{

    NewEditAlarmWindow::NewEditAlarmWindow(app::ApplicationCommon *app,
                                           std::unique_ptr<AlarmClockEditWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, style::alarmClock::window::name::newEditAlarm), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void NewEditAlarmWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setActive(gui::nav_bar::Side::Right, true);
        navBar->setActive(gui::nav_bar::Side::Center, true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::save));

        list = new gui::ListView(this,
                                 style::alarmClock::window::listView_x,
                                 style::alarmClock::window::listView_y,
                                 style::alarmClock::window::listView_w,
                                 style::alarmClock::window::listView_h,
                                 presenter->getAlarmsItemProvider(),
                                 gui::listview::ScrollBarType::None);
        setFocusItem(list);
    }

    void NewEditAlarmWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        switch (alarmAction) {
        case AlarmAction::Add:
            setTitle(utils::translate("app_alarm_clock_new_alarm_title"));
            break;
        case AlarmAction::Edit:
            setTitle(utils::translate("app_alarm_clock_edit_alarm_title"));
            break;
        }

        if (mode == gui::ShowMode::GUI_SHOW_INIT) {
            auto rec = dynamic_cast<AlarmRecordData *>(data);
            if (rec != nullptr) {
                alarmRecord = rec->getData();
            }
            presenter->loadData(alarmRecord);
        }
        else {
            presenter->loadCustomRepeat();
        }
    }

    void NewEditAlarmWindow::onClose(gui::Window::CloseReason reason)
    {
        if (reason == Window::CloseReason::ApplicationClose) {
            presenter->clearData();
        }
    }

    bool NewEditAlarmWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->saveData(alarmRecord, alarmAction);
            return true;
        }

        return false;
    }

    bool NewEditAlarmWindow::handleSwitchData(gui::SwitchData *data)
    {
        if (data == nullptr) {
            return false;
        }

        if (data->getDescription() == style::alarmClock::editAlarm) {
            alarmAction = AlarmAction::Edit;
        }
        else {
            alarmAction = AlarmAction::Add;
        }
        return true;
    }
} // namespace app::alarmClock

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditAlarmWindow.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "application-calendar/data/OptionParser.hpp"
#include <module-db/Interface/AlarmEventRecord.hpp>

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

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::save));

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

        if (mode == gui::ShowMode::GUI_SHOW_RETURN) {
            if (auto receivedData = dynamic_cast<WeekDaysRepeatData *>(data); receivedData != nullptr) {
                presenter->updateRepeat(alarmRecord, *receivedData);
                presenter->loadRepeat(alarmRecord);
            }
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

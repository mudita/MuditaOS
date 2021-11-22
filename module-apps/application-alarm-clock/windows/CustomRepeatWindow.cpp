// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomRepeatWindow.hpp"

#include <application-alarm-clock/data/AlarmsData.hpp>
#include <application-alarm-clock/widgets/AlarmClockStyle.hpp>

#include <InputEvent.hpp>

namespace app::alarmClock
{

    CustomRepeatWindow::CustomRepeatWindow(app::ApplicationCommon *app,
                                           std::unique_ptr<CustomRepeatWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, style::alarmClock::window::name::customRepeat), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void CustomRepeatWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setActive(gui::nav_bar::Side::Right, true);
        navBar->setActive(gui::nav_bar::Side::Center, true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::save));

        setTitle(utils::translate("app_alarm_clock_custom_repeat_title"));
        list = new gui::ListView(this,
                                 style::alarmClock::window::listView_x,
                                 style::alarmClock::window::listView_y,
                                 style::alarmClock::window::listView_w,
                                 style::alarmClock::window::listView_h,
                                 presenter->getItemProvider(),
                                 gui::listview::ScrollBarType::None);
        setFocusItem(list);
    }

    void CustomRepeatWindow::onClose(gui::Window::CloseReason reason)
    {
        if (reason != CloseReason::PhoneLock) {
            presenter->eraseProviderData();
        }
    }

    void CustomRepeatWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        presenter->loadData();
    }

    bool CustomRepeatWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->saveData();
            application->switchWindow(style::alarmClock::window::name::newEditAlarm, gui::ShowMode::GUI_SHOW_RETURN);
            return true;
        }
        return false;
    }
} // namespace app::alarmClock

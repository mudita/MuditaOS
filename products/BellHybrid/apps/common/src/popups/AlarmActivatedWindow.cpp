// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ApplicationCommon.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <common/TimeUtils.hpp>
#include <common/data/StyleCommon.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <popups/Popups.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>
#include <application-bell-alarm/ApplicationBellAlarmNames.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <WindowsStack.hpp>

namespace
{
    constexpr unsigned int y_position = 22U;
} // namespace

namespace gui
{
    AlarmActivatedWindow::AlarmActivatedWindow(app::ApplicationCommon *app,
                                               std::shared_ptr<app::popup::AlarmActivatedPresenter> presenter)
        : WindowWithTimer(app, popup::window::alarm_activated_window), app::popup::AlarmActivatedContract::View(
                                                                           std::move(presenter))
    {
        getPresenter()->attach(this);
        getPresenter()->updateAlarmModel([&]() {
            setAlarmTime(getPresenter()->getAlarmTime());
            getPresenter()->activate();
        });
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            returnToPreviousWindow();
            return true;
        };
    }

    void AlarmActivatedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        icon = new Icon(this, 0, 0, style::window_width, style::window_height, "big_alarm_W_G", {});
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        icon->image->setMargins({0, bell_style::popup_icon_top_margin, 0, bell_style::popup_icon_bottom_margin});
        icon->text->setFont(style::window::font::verybiglight);
    }

    void AlarmActivatedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
    }

    void AlarmActivatedWindow::returnToPreviousWindow()
    {
        application->windowsStack().pop(popup::window::alarm_deactivated_window);

        if (application->windowsStack().pop(gui::window::name::bellAlarmSet)) {
            app::manager::Controller::sendAction(
                application,
                app::manager::actions::Launch,
                std::make_unique<app::ApplicationLaunchData>(app::applicationBellName));
        }
        else {
            application->returnToPreviousWindow();
        }
    }

    bool AlarmActivatedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            returnToPreviousWindow();
            return true;
        }
        return false;
    }

    void AlarmActivatedWindow::setAlarmTime(time_t alarmTime)
    {
        icon->text->setRichText(utils::time::getBottomDescription(
            utils::time::calculateMinutesDifference(alarmTime, utils::time::getCurrentTime())));
    }
} /* namespace gui */

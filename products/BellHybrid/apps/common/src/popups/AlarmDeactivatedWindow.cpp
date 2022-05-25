// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ApplicationCommon.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <apps-common/popups/Popups.hpp>
#include <common/data/StyleCommon.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>
#include <application-bell-alarm/ApplicationBellAlarmNames.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <WindowsStack.hpp>

namespace gui
{
    AlarmDeactivatedWindow::AlarmDeactivatedWindow(app::ApplicationCommon *app,
                                                   std::shared_ptr<app::popup::AlarmActivatedPresenter> presenter)
        : WindowWithTimer(app, popup::window::alarm_deactivated_window), app::popup::AlarmActivatedContract::View(
                                                                             std::move(presenter))
    {
        getPresenter()->attach(this);
        buildInterface();
        getPresenter()->updateAlarmModel([&]() { getPresenter()->deactivate(); });

        timerCallback = [this](Item &, sys::Timer &) {
            returnToPreviousWindow();
            return true;
        };
    }

    bool AlarmDeactivatedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            returnToPreviousWindow();
            return true;
        }
        return false;
    }

    void AlarmDeactivatedWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "big_no-alarm_W_G",
                        utils::translate("app_bell_alarm_deactivated"));
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        icon->image->setMargins({0, bell_style::popup_icon_top_margin, 0, bell_style::popup_icon_bottom_margin});
        icon->text->setFont(style::window::font::verybiglight);
    }

    void AlarmDeactivatedWindow::returnToPreviousWindow()
    {
        application->windowsStack().pop(popup::window::alarm_activated_window);

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
} /* namespace gui */

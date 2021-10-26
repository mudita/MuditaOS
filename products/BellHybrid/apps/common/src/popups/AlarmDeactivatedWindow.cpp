// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ApplicationCommon.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <apps-common/popups/Popups.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>
#include <application-bell-alarm/ApplicationBellAlarmNames.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

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
        icon->text->setFont(style::window::font::verybiglight);
    }

    void AlarmDeactivatedWindow::returnToPreviousWindow()
    {
        app::manager::Controller::sendAction(application,
                                             app::manager::actions::AbortPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::AlarmActivated));

        if (application->getPrevWindow() == gui::window::name::bellAlarmSet) {
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

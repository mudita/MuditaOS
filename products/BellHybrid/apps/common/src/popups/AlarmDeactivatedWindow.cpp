// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/popups/AlarmDeactivatedWindow.hpp>

#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <apps-common/popups/Popups.hpp>
#include <i18n/i18n.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    AlarmDeactivatedWindow::AlarmDeactivatedWindow(app::ApplicationCommon *app)
        : WindowWithTimer(app, popup::window::alarm_deactivated_window)
    {
        buildInterface();
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
        bottomBar->setVisible(false);

        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "bell_alarm_deactivated",
                        utils::translate("app_bell_alarm_deactivated"));
        icon->text->setFont(style::window::font::verybiglight);
    }
    void AlarmDeactivatedWindow::returnToPreviousWindow()
    {
        app::manager::Controller::sendAction(application,
                                             app::manager::actions::AbortPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::AlarmActivated));
        application->returnToPreviousWindow();
    }
} /* namespace gui */

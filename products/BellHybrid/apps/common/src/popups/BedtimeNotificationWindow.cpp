﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <apps-common/popups/Popups.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <common/popups/BedtimeNotificationWindow.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    BedtimeNotificationWindow::BedtimeNotificationWindow(app::ApplicationCommon *app)
        : WindowWithTimer(app, popup::window::bedtime_notification_window, bedtimeNotificationTimeout),
          app::AsyncCallbackReceiver{app}, app{app}
    {
        buildInterface();
        timerCallback = [this](Item &, sys::Timer &) {
            returnToPreviousWindow();
            return true;
        };
    }

    void BedtimeNotificationWindow::buildInterface()
    {
        AppWindow::buildInterface();
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        setTitle("");

        icon = new Icon(this,
                        style::window::default_left_margin,
                        style::window::default_vertical_pos,
                        style::window::default_body_width,
                        style::window::default_body_height,
                        "",
                        "");

        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));
        icon->text->setFont(style::window::font::verybiglight);
    }
    void BedtimeNotificationWindow::onBeforeShow(ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        icon->text->setRichText(utils::translate(bedtimeNotificationText));
        icon->image->set(bedtimeNotificationIcon);
        icon->resizeItems();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setActive(nav_bar::Side::Right, false);
    }

    void BedtimeNotificationWindow::returnToPreviousWindow()
    {
        detachTimerIfExists();
        app::manager::Controller::sendAction(
            application,
            app::manager::actions::AbortPopup,
            std::make_unique<gui::PopupRequestParams>(gui::popup::ID::BedtimeNotification));
        application->returnToPreviousWindow();
    }

    bool BedtimeNotificationWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            returnToPreviousWindow();
            return true;
        }
        return false;
    }
} /* namespace gui */

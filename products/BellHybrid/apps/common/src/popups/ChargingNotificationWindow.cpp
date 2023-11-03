// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <apps-common/popups/Popups.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <common/popups/ChargingNotificationWindow.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-appmgr/Controller.hpp>
#include "EventStore.hpp"

namespace
{
    constexpr auto chargingIcon                = "big_bell_battery_charging_W_G";
    constexpr auto chargingText                = "app_bell_charging_notification";
    constexpr auto chargingDoneText            = "app_bell_charging_done_notification";
    constexpr auto chargingNotificationTimeout = std::chrono::seconds{5};

} // namespace
namespace gui
{
    ChargingNotificationWindow::ChargingNotificationWindow(app::ApplicationCommon *app, const std::string &windowName)
        : WindowWithTimer(app, windowName, chargingNotificationTimeout), app::AsyncCallbackReceiver{app}, app{app}
    {
        buildInterface();
        timerCallback = [this](Item &, sys::Timer &) {
            returnToPreviousWindow();
            return true;
        };
    }

    void ChargingNotificationWindow::buildInterface()
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

    bool ChargingNotificationWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            returnToPreviousWindow();
            return true;
        }
        return false;
    }
    void ChargingNotificationWindow::returnToPreviousWindow()
    {
        if (application->getPreviousWindow()) {
            application->returnToPreviousWindow();
        }
    }

    ChargingDoneNotificationWindow::ChargingDoneNotificationWindow(app::ApplicationCommon *app)
        : ChargingNotificationWindow(app, gui::popup::window::charging_done_notification_window)
    {}

    void ChargingDoneNotificationWindow::onBeforeShow(ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        icon->text->setRichText(utils::translate(chargingDoneText));
        icon->image->set(chargingIcon);
        icon->resizeItems();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setActive(nav_bar::Side::Right, false);
    }

    void ChargingDoneNotificationWindow::returnToPreviousWindow()
    {
        app::manager::Controller::sendAction(
            application,
            app::manager::actions::AbortPopup,
            std::make_unique<gui::PopupRequestParams>(gui::popup::ID::ChargingDoneNotification));
        ChargingNotificationWindow::returnToPreviousWindow();
    }

    ChargingInProgressNotificationWindow::ChargingInProgressNotificationWindow(app::ApplicationCommon *app)
        : ChargingNotificationWindow(app, gui::popup::window::charging_notification_window)
    {}

    void ChargingInProgressNotificationWindow::onBeforeShow(ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        icon->text->setRichText(std::to_string(Store::Battery::get().level) + "% " + utils::translate(chargingText));
        icon->image->set(chargingIcon);
        icon->resizeItems();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setActive(nav_bar::Side::Right, false);
    }

    void ChargingInProgressNotificationWindow::returnToPreviousWindow()
    {
        app::manager::Controller::sendAction(
            application,
            app::manager::actions::AbortPopup,
            std::make_unique<gui::PopupRequestParams>(gui::popup::ID::ChargingNotification));
        ChargingNotificationWindow::returnToPreviousWindow();
    }

} /* namespace gui */

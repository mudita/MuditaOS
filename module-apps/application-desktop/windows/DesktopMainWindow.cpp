/*
 * @file DesktopMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "../ApplicationDesktop.hpp"
#include "../data/LockPhoneData.hpp"
#include "Alignment.hpp"
#include "Common.hpp"
#include "BottomBar.hpp"
#include "DesktopMainWindow.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "gui/widgets/Image.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include <UiCommonActions.hpp>

#include <gui/tools/Common.hpp>
#include "i18/i18.hpp"
#include "log/log.hpp"
#include <Span.hpp>
#include <Style.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <cassert>
#include <service-appmgr/ApplicationManager.hpp>
#include <time/time_conversion.hpp>

namespace style
{
    const auto design_time_offset          = 106;
    const auto design_time_h               = 96;
    const auto design_day_offset           = 204;
    const auto design_day_h                = 51;
    const auto design_border_offset        = 20;
    const auto design_option_span          = 8;
    const auto design_notifications_offset = 284;
    const auto digit_normal_size              = 13;
    const auto notification_icon_unified_size = 35;
}; // namespace style

namespace gui
{

    void DesktopMainWindow::buildInterface()
    {

        auto ttime = utils::time::Time();
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        topBar->setActive(
            {{TopBar::Elements::SIGNAL, true}, {TopBar::Elements::LOCK, true}, {TopBar::Elements::BATTERY, true}});

        time = new gui::Label(this, 0, style::design_time_offset, style::window_width, style::design_time_h);
        time->setFilled(false);
        time->setBorderColor(gui::ColorNoColor);
        time->setFont(style::window::font::supersizemelight);
        time->setText(ttime);
        time->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        dayText = new gui::Label(this, 0, style::design_day_offset, style::window_width, style::design_day_h);
        dayText->setFilled(false);
        dayText->setBorderColor(gui::ColorNoColor);
        dayText->setFont(style::window::font::biglight);
        dayText->setText(ttime.day() + ", " + ttime.str("%d %b"));
        dayText->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        setVisibleState();
    }

    void DesktopMainWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void DesktopMainWindow::invalidate() noexcept
    {
        time          = nullptr;
        dayText       = nullptr;
        notifications = nullptr;
    }

    DesktopMainWindow::DesktopMainWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_main_window)
    {
        buildInterface();
    }

    void DesktopMainWindow::setVisibleState()
    {
        auto app = dynamic_cast<app::ApplicationDesktop *>(application);
        assert(app != nullptr);

        if (app->getScreenLocked()) {
            bottomBar->restore();
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
            topBar->setActive(TopBar::Elements::LOCK, true);
            inputCallback = nullptr;
            setFocusItem(nullptr);
            erase(notifications);
        }
        else {
            auto app = dynamic_cast<app::ApplicationDesktop *>(application);
            assert(app);
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_menu"));
            topBar->setActive(TopBar::Elements::LOCK, false);
            if (!fillNotifications(app)) {
                LOG_ERROR("Couldn't fit in all notifications");
            }
            if (app->need_sim_select && Store::GSM::get()->sim == Store::GSM::SIM::SIM_UNKNOWN) {
                sapm::ApplicationManager::messageSwitchApplication(
                    this->application, app::name_settings, app::sim_select, nullptr);
            }
        }
    }

    void DesktopMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        // update time
        time->setText(topBar->getTimeString());

        // check if there was a signal to lock the pone due to inactivity.
        if ((data != nullptr) && (data->getDescription() == "LockPhoneData")) {
            auto app = dynamic_cast<app::ApplicationDesktop *>(application);
            if (!app) {
                return;
            }
            app->setScreenLocked(true);

            LockPhoneData *lockData = reinterpret_cast<LockPhoneData *>(data);
            lockTimeoutApplilcation = lockData->getPreviousApplication();

            reinterpret_cast<app::ApplicationDesktop *>(application)->setSuspendFlag(true);
        }

        setVisibleState();
    }

    bool DesktopMainWindow::onInput(const InputEvent &inputEvent)
    {
        app::ApplicationDesktop *app = dynamic_cast<app::ApplicationDesktop *>(application);
        if (app == nullptr) {
            LOG_ERROR("not ApplicationDesktop");
            return AppWindow::onInput(inputEvent);
        }

        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());

        // process shortpress
        if (inputEvent.isShortPress()) {
            if (app->getScreenLocked()) {
                // if enter was pressed
                if (enter_cache.cached() && inputEvent.is(KeyCode::KEY_PND)) {
                    // if interval between enter and pnd keys is less than time defined for unlocking
                    // display pin lock screen or simply refresh current window to update labels
                    if (app->getPinLocked()) {
                        std::unique_ptr<LockPhoneData> data =
                            std::make_unique<LockPhoneData>(LockPhoneData::Request::Pin);
                        // if there was no application on to before closing proceed normally to pin protection
                        if (lockTimeoutApplilcation.empty()) {
                            application->switchWindow(app::window::name::desktop_pin_lock, std::move(data));
                            return true;
                        }
                        else {
                            data->setPrevApplication(lockTimeoutApplilcation);
                            lockTimeoutApplilcation = "";
                            application->switchWindow(app::window::name::desktop_pin_lock, std::move(data));
                            return true;
                        }
                    }
                    else {

                        // if phone was locked by user show unlocked main window
                        if (lockTimeoutApplilcation.empty()) {
                            app->setScreenLocked(false);
                            setVisibleState();
                            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                            return true;
                        }
                        // if there was application on top when timeout occurred
                        else {
                            lockTimeoutApplilcation = "";
                            sapm::ApplicationManager::messageSwitchPreviousApplication(application);
                        }
                    }
                }
                // not locked && not unlock -> show prompt
                if ((app->getPinLocked() == false) && ((!inputEvent.is(KeyCode::KEY_ENTER)) ||
                                                       (inputEvent.is(KeyCode::KEY_ENTER) && enter_cache.cached()))) {
                    if (inputEvent.is(KeyCode::KEY_ENTER) && enter_cache.cached()) {
                        enter_cache.clear();
                    }
                    application->switchWindow(app::window::name::desktop_pin_lock,
                                              std::make_unique<LockPhoneData>(LockPhoneData::Request::NoPin));
                    return true;
                }
                if (!inputEvent.is(KeyCode::KEY_ENTER)) { // TODO TUTAJ BREAKPOINT
                    application->switchWindow(app::window::name::desktop_pin_lock,
                                              std::make_unique<LockPhoneData>(LockPhoneData::Request::ShowPrompt));
                    return true;
                }
                if (enter_cache.storeEnter(inputEvent)) {}
            }
            // screen is unlocked
            else {
                // pressing enter moves user to menu screen
                if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
                    application->switchWindow("MenuWindow");
                    return true;
                }
                // if numeric key was pressed record that key and send it to call application
                else if (code != 0) {
                    return app::prepareCall(application, std::string(1, static_cast<char>(code)));
                }
            }
        }
        else if (inputEvent.isLongPress()) {
            // long press of # locks screen if it was unlocked
            if ((inputEvent.keyCode == KeyCode::KEY_PND) && (app->getScreenLocked() == false)) {
                app->setScreenLocked(true);
                setVisibleState();
                app->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                app->setSuspendFlag(true);
                return true;
            }
            // long press of right function button move user to power off window
            else if (inputEvent.keyCode == KeyCode::KEY_RF) {
                application->switchWindow("PowerOffWindow");
                return true;
            }
            // long press of '0' key is translated to '+'
            else if (inputEvent.keyCode == KeyCode::KEY_0) {
                return app::prepareCall(application, "+");
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    void DesktopMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    bool DesktopMainWindow::updateTime(const UTF8 &timeStr)
    {
        auto ret = AppWindow::updateTime(timeStr);
        time->setText(topBar->getTimeString());
        return ret;
    }
    bool DesktopMainWindow::updateTime(const uint32_t &timestamp, bool mode24H)
    {
        auto ret = AppWindow::updateTime(timestamp, mode24H);
        time->setText(topBar->getTimeString());
        return ret;
    }

    std::list<DrawCommand *> DesktopMainWindow::buildDrawList()
    {
        time->setText(topBar->getTimeString());
        return gui::AppWindow::buildDrawList();
    }

    auto add_image_inactive(UTF8 img)
    {
        auto thumbnail        = new gui::Image(img);
        thumbnail->activeItem = false;
        return thumbnail;
    }

    auto add_box_icon(UTF8 icon)
    {
        auto thumbnail = add_image_inactive(icon);
        thumbnail->setMinimumWidth(style::notification_icon_unified_size);
        thumbnail->setMargins(
            gui::Margins(style::window::default_left_margin, 0, style::window::default_right_margin, 0));
        return thumbnail;
    }

    /// for now notifications are like that: `^<span>[icon]<span>[dumb text]       [dot image] [number of
    /// notifications]<span>$`
    auto add_notification(BoxLayout *layout,
                          UTF8 icon,
                          UTF8 name,
                          UTF8 indicator,
                          std::function<bool()> showCallback,
                          std::function<bool()> clearCallback) -> bool
    {
        // 1. create hbox for all elements
        auto el = new gui::HBox(nullptr, 0, 0, style::window::default_body_width, style::window::label::default_h);
        el->setAlignment(Alignment(gui::Alignment::Vertical::Center));

        auto text = new gui::Label();
        text->setMaximumSize(el->area().w, Axis::X);
        text->setText(name);
        text->setFont(style::window::font::medium);
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        text->setPenWidth(style::window::default_border_no_focus_w);
        text->activeItem = false;

        auto number = new gui::Text();
        if (indicator.length() > 2) {
            const UTF8 max_notification_value = "99+";
            number->setText(max_notification_value);
            number->setMinimumWidth(max_notification_value.length() * style::digit_normal_size);
        }
        else {
            number->setText(indicator);
            number->setMinimumWidth(indicator.length() * style::digit_normal_size);
        }
        number->setFont(style::window::font::mediumbold);
        number->setPenWidth(style::window::default_border_no_focus_w);
        number->setMargins(gui::Margins(0, 0, style::window::default_right_margin, 0));
        number->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        number->activeItem = false;

        // 2. Add all elements to hbox layout
        el->addWidget(add_box_icon(icon));
        el->addWidget(text);
        el->addWidget(add_image_inactive("dot_12px_hard_alpha_W_G"));
        el->addWidget(number);

        // 3. Set hbox layout properties
        el->setPenWidth(style::window::default_border_no_focus_w);
        el->setPenFocusWidth(style::window::default_border_focus_w);
        el->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
        el->inputCallback = [showCallback, clearCallback](Item &, const InputEvent &event) -> bool {
            if (event.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == KeyCode::KEY_LF && showCallback) {
                return showCallback();
            }
            if (event.keyCode == KeyCode::KEY_RF && clearCallback) {
                return clearCallback();
            }
            return false;
        };

        layout->addWidget(el);
        if (el->visible) {
            // space between next notifications to show
            layout->addWidget(new gui::Span(Axis::Y, style::design_option_span));
        }

        return el->visible;
    }

    auto DesktopMainWindow::fillNotifications(app::ApplicationDesktop *app) -> bool
    {
        bottomBar->restore();
        erase(notifications);
        // 1. create notifications box
        notifications = new gui::VBox(nullptr,
                                      0,
                                      style::design_notifications_offset,
                                      style::window_width,
                                      bottomBar->widgetArea.pos(Axis::Y) - style::design_notifications_offset);
        notifications->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        notifications->setPenWidth(style::window::default_border_no_focus_w);
        notifications->setPenFocusWidth(style::window::default_border_no_focus_w);
        this->addWidget(notifications);
        if (!notifications->visible) {
            LOG_ERROR("Can't fit notifications box!");
            return false;
        }

        // 2. actually fill it in
        if (app->notifications.notSeen.Calls > 0) {
            add_notification(
                notifications,
                "phone",
                utils::localize.get("app_desktop_missed_calls"),
                std::to_string(app->notifications.notSeen.Calls),
                [app]() -> bool { return app->showCalls(); },
                [app]() -> bool { return app->clearCallsNotification(); });
        }
        if (app->notifications.notSeen.SMS > 0) {
            add_notification(
                notifications,
                "mail",
                utils::localize.get("app_desktop_unread_messages"),
                std::to_string(app->notifications.notSeen.SMS),
                [this]() -> bool {
                    return sapm::ApplicationManager::messageSwitchApplication(
                        application, app::name_messages, gui::name::window::main_window, nullptr);
                },
                [app]() -> bool { return app->clearMessagesNotification(); });
        }
        bottomBar->store();
        if (app->notifications.notSeen.areEmpty() != true) {
            setFocusItem(notifications);
            bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_show"));
            bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_desktop_clear"));
        }
        else {
            bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_calls"));
            inputCallback = [app](Item &, const InputEvent &inputEvent) -> bool {
                if (inputEvent.state == InputEvent::State::keyReleasedShort &&
                    inputEvent.keyCode == gui::KeyCode::KEY_LF) {
                    return app->showCalls();
                }
                return false;
            };
        }
        return true;
    }

} /* namespace gui */

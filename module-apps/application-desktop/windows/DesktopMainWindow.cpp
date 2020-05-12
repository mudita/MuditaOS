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
#include "DesktopMainWindow.hpp"
#include "application-calllog/ApplicationCallLog.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "gui/widgets/Image.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include <UiCommonActions.hpp>

#include "Common.hpp"
#include "i18/i18.hpp"
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
        time->setAlignment(Alignment::ALIGN_HORIZONTAL_CENTER);

        dayText = new gui::Label(this, 0, style::design_day_offset, style::window_width, style::design_day_h);
        dayText->setFilled(false);
        dayText->setBorderColor(gui::ColorNoColor);
        dayText->setFont(style::window::font::biglight);
        dayText->setText(ttime.day() + ", " + ttime.str("%d %b"));
        dayText->setAlignment(Alignment::ALIGN_HORIZONTAL_CENTER);

        setVisibleState();
    }

    void DesktopMainWindow::destroyInterface()
    {
        erase();
    }

    DesktopMainWindow::DesktopMainWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_main_window)
    {
        buildInterface();
    }

    void DesktopMainWindow::setVisibleState()
    {

        auto app = dynamic_cast<app::ApplicationDesktop *>(application);
        if (!app) {
            return;
        }

        if (app->getScreenLocked()) {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
            topBar->setActive(TopBar::Elements::LOCK, true);
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

        // do nothing
        if (inputEvent.state == InputEvent::State::keyReleasedShort && inputEvent.keyCode == KeyCode::KEY_RF)
            return false;

        app::ApplicationDesktop *app = dynamic_cast<app::ApplicationDesktop *>(application);
        if (app == nullptr) {
            LOG_ERROR("not ApplicationDesktop");
            return AppWindow::onInput(inputEvent);
        }

        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());

        // process shortpress
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {

            if (app->getScreenLocked()) {
                // if enter was pressed
                if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
                    unlockStartTime = xTaskGetTickCount();
                    enterPressed    = true;
                }
                else if ((inputEvent.keyCode == KeyCode::KEY_PND) && enterPressed) {
                    // if interval between enter and pnd keys is less than time defined for unlocking
                    if (xTaskGetTickCount() - unlockStartTime < unclockTime) {
                        // display pin lock screen or simply refresh current window to update labels
                        if (app->getPinLocked())
                            // if there was no application on to before closing proceed normally to pin protection
                            // window.
                            if (lockTimeoutApplilcation.empty()) {
                                application->switchWindow("PinLockWindow");
                            }
                            else {
                                std::unique_ptr<LockPhoneData> data = std::make_unique<LockPhoneData>();
                                data->setPrevApplication(lockTimeoutApplilcation);
                                lockTimeoutApplilcation = "";
                                application->switchWindow("PinLockWindow", std::move(data));
                            }

                        else {

                            // if phone was locked by user show unlocked main window
                            if (lockTimeoutApplilcation.empty()) {
                                app->setScreenLocked(false);
                                setVisibleState();
                                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                            }
                            // if there was application on top when timeout occurred
                            else {
                                lockTimeoutApplilcation = "";
                                sapm::ApplicationManager::messageSwitchPreviousApplication(application);
                            }
                        }
                    }
                    enterPressed = false;
                }
                else {
                    enterPressed = false;
                }
            }
            // screen is unlocked
            else {
                // pressing enter moves user to menu screen
                if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
                    application->switchWindow("MenuWindow");
                }
                // if numeric key was pressed record that key and send it to call application
                else if (code != 0) {
                    return app::prepare_call(application, std::string(1, static_cast<char>(code)));
                }
            }
        }
        else if (inputEvent.state == InputEvent::State::keyReleasedLong) {
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
                return app::prepare_call(application, std::to_string('+'));
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

    auto add_box_icon(gui::BoxLayout *layout, UTF8 icon)
    {
        auto thumbnail = new gui::Image(icon);
        center(layout, thumbnail, Axis::Y);
        thumbnail->activeItem = false;
        layout->addWidget(thumbnail);
    }

    /// for now notifications are like that: `^<span>[icon]<span>[dumb text]       [dot image] [number of
    /// notifications]<span>$`
    auto add_notification(BoxLayout *layout, UTF8 icon, UTF8 name, UTF8 indicator, std::function<bool()> callback)
        -> bool
    {
        const auto text_normal_size        = 200;
        const auto size_needed_for_2digits = 30;
        // 1. create hbox for all elements
        auto el   = new gui::HBox(nullptr, 0, 0, layout->getWidth(), style::window::label::default_h);
        auto text = new gui::Label(nullptr, 0, 0, text_normal_size, style::window::label::default_h, "");
        text->setMaximumSize(el->area().w, Axis::X);
        text->setText(name);
        text->setFont(style::window::font::medium);
        text->setAlignment(Alignment::ALIGN_VERTICAL_CENTER);
        text->setPenWidth(style::window::default_border_no_focus_w);
        text->activeItem = false;

        auto number = new gui::Label();
        number->setText(indicator);
        number->setFont(style::window::font::mediumbold);
        number->setPenWidth(style::window::default_border_no_focus_w);
        number->setSize(size_needed_for_2digits, el->area().h);
        number->setMinimumWidth(size_needed_for_2digits);
        number->setAlignment(Alignment::ALIGN_VERTICAL_CENTER | Alignment::ALIGN_HORIZONTAL_RIGHT);
        number->activeItem = false;
        // 2. Add all elements to hbox layout
        new gui::Span(el, Axis::X, style::design_border_offset);
        add_box_icon(el, icon);
        new gui::Span(el, Axis::X, style::design_border_offset);
        el->addWidget(text);
        add_box_icon(el, "dot_12px_soft");
        el->addWidget(number);
        // box right inner margin
        new gui::Span(el, Axis::X, style::design_border_offset);
        // 3. Set hbox layout properties
        el->setPenWidth(style::window::default_border_no_focus_w);
        el->setPenFocusWidth(style::window::default_border_focucs_w);
        el->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
        el->inputCallback = [callback](Item &, const InputEvent &event) {
            if (event.state == InputEvent::State::keyReleasedShort && event.keyCode == KeyCode::KEY_LF) {
                return callback();
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
        erase(notifications);
        // 1. create notifications box
        notifications = new gui::VBox(nullptr,
                                      style::window::default_left_margin,
                                      style::design_notifications_offset,
                                      style::window_width - 2 * style::window::default_left_margin,
                                      bottomBar->widgetArea.pos(Axis::Y) - style::design_notifications_offset);
        notifications->setPenWidth(style::window::default_border_no_focus_w);
        notifications->setPenFocusWidth(style::window::default_border_no_focus_w);
        this->addWidget(notifications);
        if (!notifications->visible) {
            LOG_ERROR("Can't fit notifications box!");
            return false;
        }

        // 2. actually fill it in
        if (app->notifications.notSeenCalls) {
            add_notification(notifications,
                             "phone",
                             utils::localize.get("app_desktop_missed_calls"),
                             std::to_string(app->notifications.notSeenCalls),
                             [this]() {
                                 return sapm::ApplicationManager::messageSwitchApplication(
                                     application, app::CallLogAppStr, gui::name::window::main_window, nullptr);
                             });
        }
        if (app->notifications.notSeenSMS) {
            add_notification(notifications,
                             "mail",
                             utils::localize.get("app_desktop_unread_messages"),
                             std::to_string(app->notifications.notSeenSMS),
                             [this]() {
                                 return sapm::ApplicationManager::messageSwitchApplication(
                                     application, app::name_messages, gui::name::window::main_window, nullptr);
                             });
        }
        setFocusItem(notifications);
        return true;
    }

} /* namespace gui */

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>

#include "Alignment.hpp"
#include "BottomBar.hpp"
#include "Common.hpp"
#include "DesktopMainWindow.hpp"
#include "GuiTimer.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/data/LockPhoneData.hpp"
#include "application-desktop/data/Style.hpp"
#include "application-desktop/widgets/NotificationsBox.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "gui/widgets/Image.hpp"
#include <service-appmgr/Controller.hpp>
#include <service-time/ServiceTime.hpp>
#include <service-time/TimeMessage.hpp>
#include <UiCommonActions.hpp>

#include "i18/i18.hpp"
#include "log/log.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <cassert>
#include <time/time_conversion.hpp>

namespace gui
{
    void DesktopMainWindow::buildInterface()
    {
        auto ttime = utils::time::Time();
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        topBar->setActive(
            {{TopBar::Elements::SIGNAL, true}, {TopBar::Elements::LOCK, true}, {TopBar::Elements::BATTERY, true}});

        using namespace style::desktop;

        time = new gui::Label(this, timeLabel::X, timeLabel::Y, timeLabel::Width, timeLabel::Hight);
        time->setFilled(false);
        time->setBorderColor(gui::ColorNoColor);
        time->setFont(style::window::font::supersizemelight);
        time->setText(ttime);
        time->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        dayText = new gui::Label(this, dayLabel::X, dayLabel::Y, dayLabel::Width, dayLabel::Hight);
        dayText->setFilled(false);
        dayText->setBorderColor(gui::ColorNoColor);
        dayText->setFont(style::window::font::biglight);
        dayText->setText(ttime.day() + ", " + ttime.str("%d %b"));
        dayText->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        activatedCallback = [this](Item &) {
            application->switchWindow(app::window::name::desktop_menu);
            return true;
        };

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

        preBuildDrawListHook = [this](std::list<Command> &cmd) {
            if (time != nullptr) {
                time->setText(topBar->getTimeString());
            }
        };
    }

    void DesktopMainWindow::setVisibleState()
    {
        auto app = getAppDesktop();
        if (app->lockHandler.isScreenLocked()) {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            bottomBar->setActive(BottomBar::Side::RIGHT, false);
            topBar->setActive(TopBar::Elements::LOCK, true);
            inputCallback = nullptr;
            setFocusItem(nullptr);
            buildNotifications(app);

            sys::Bus::SendUnicast(
                std::make_shared<TimersProcessingStopMessage>(), service::name::service_time, application);
        }
        else {
            topBar->setActive(TopBar::Elements::LOCK, false);

            if (!buildNotifications(app)) {
                LOG_ERROR("Couldn't fit in all notifications");
            }
            setActiveState(app);

            if (app->need_sim_select && Store::GSM::get()->sim == Store::GSM::SIM::SIM_UNKNOWN) {
                app::manager::Controller::switchApplication(
                    this->application, app::name_settings, app::sim_select, nullptr);
            }

            sys::Bus::SendUnicast(
                std::make_shared<TimersProcessingStartMessage>(), service::name::service_time, application);
        }
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void DesktopMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        // update time
        time->setText(topBar->getTimeString());
        // check if there was a signal to lock the pone due to inactivity.
        if ((data != nullptr) && (data->getDescription() == "LockPhoneData")) {
            auto app = getAppDesktop();
            if (app->lockHandler.isScreenLocked()) {
                return;
            }
            auto *lockData          = dynamic_cast<LockPhoneData *>(data);
            lockTimeoutApplilcation = lockData->getPreviousApplication();
            application->setSuspendFlag(true);
        }

        if (mode == ShowMode::GUI_SHOW_RETURN) {
            auto lock = getScreenLock();
            if (lock->getState() == PinLock::LockState::Unlocked) {
                setVisibleState();
                return;
            }
            switchToPinLockWindow(std::move(lock));
        }

        setVisibleState();
    }

    bool DesktopMainWindow::processLongPressEvent(const InputEvent &inputEvent)
    {
        auto app = getAppDesktop();

        if (!app->lockHandler.isScreenLocked()) {
            if (inputEvent.is(KeyCode::KEY_PND)) {
                app->lockHandler.lockScreen();
                setVisibleState();
                application->setSuspendFlag(true);
                return true;
            }
            // long press of '0' key is translated to '+'
            else if (inputEvent.is(KeyCode::KEY_0)) {
                return app::prepareCall(application, "+");
            }
        }

        if (inputEvent.is(KeyCode::KEY_RF)) {
            application->switchWindow("PowerOffWindow");
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool DesktopMainWindow::processShortPressEventOnUnlocked(const InputEvent &inputEvent)
    {
        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());
        // if numeric key was pressed record that key and send it to call application
        if (code != 0) {
            return app::prepareCall(application, std::string(1, static_cast<char>(code)));
        }
        else if (inputEvent.is(KeyCode::KEY_UP) && focusItem == nullptr) {
            setFocusItem(notifications);
            notifications->navigateToBottom();
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_DOWN) && focusItem == nullptr) {
            setFocusItem(notifications);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool DesktopMainWindow::processShortPressEventOnLocked(const InputEvent &inputEvent)
    {
        // if enter was pressed
        if (enter_cache.cached() && inputEvent.is(KeyCode::KEY_PND)) {
            // if interval between enter and pnd keys is less than time defined for unlocking
            // display pin lock screen or simply refresh current window to update labels

            auto lock = getScreenLock();
            // if there is no pin,
            if (lock->getMaxPinSize() == 0) {
                lock->verify();
                return true;
            }

            switchToPinLockWindow(std::move(lock));
            return true;
        }
        else if (enter_cache.storeEnter(inputEvent)) {
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        else if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        application->switchWindow(app::window::name::desktop_locked);
        return true;
    }

    bool DesktopMainWindow::onInput(const InputEvent &inputEvent)
    {
        auto *app = getAppDesktop();

        if (inputEvent.isLongPress()) {
            return processLongPressEvent(inputEvent);
        }
        else if (inputEvent.isShortPress()) {
            if (app->lockHandler.isScreenLocked()) {
                return processShortPressEventOnLocked(inputEvent);
            }
            else {
                return processShortPressEventOnUnlocked(inputEvent);
            }
        }
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

    auto DesktopMainWindow::buildNotifications(app::ApplicationDesktop *app) -> bool
    {
        erase(notifications);
        using namespace style::desktop;
        notifications = new NotificationsBox(this,
                                             notifications::X,
                                             notifications::Y,
                                             notifications::Width,
                                             bottomBar->widgetArea.pos(Axis::Y) - notifications::Y);

        addWidget(notifications);
        if (!notifications->visible) {
            LOG_ERROR("Can't fit notifications box!");
            return false;
        }

        auto onNotificationFocus = [this](bool isFocused) -> void {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_show"), isFocused);
            bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_desktop_clear"), isFocused);
            bottomBar->setActive(BottomBar::Side::LEFT, !isFocused);
        };

        if (app->notifications.notSeen.Calls > 0) {
            notifications->addNotification(
                "phone",
                utils::localize.get("app_desktop_missed_calls"),
                std::to_string(app->notifications.notSeen.Calls),
                [app]() -> bool { return app->showCalls(); },
                [app]() -> bool { return app->clearCallsNotification(); },
                onNotificationFocus);
        }
        if (app->notifications.notSeen.SMS > 0) {
            notifications->addNotification(
                "mail",
                utils::localize.get("app_desktop_unread_messages"),
                std::to_string(app->notifications.notSeen.SMS),
                [this]() -> bool {
                    return app::manager::Controller::switchApplication(
                        application, app::name_messages, gui::name::window::main_window, nullptr);
                },
                [app, this]() -> bool { return app->clearMessagesNotification(); },
                onNotificationFocus);
        }

        notifications->focusChangedCallback = [this, app](Item &) -> bool {
            if (notifications->focus == false) {
                LOG_DEBUG("Notification box lost focus");
                setFocusItem(nullptr);
                setActiveState(app);
                return true;
            }
            return false;
        };

        return true;
    }
    auto DesktopMainWindow::setActiveState(app::ApplicationDesktop *app) -> bool
    {
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_menu"));
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_desktop_calls"));
        auto hasNotifications = !app->notifications.notSeen.areEmpty();
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_desktop_clear_all"), hasNotifications);

        inputCallback = [this, app, hasNotifications](Item &, const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortPress()) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_RF) && hasNotifications) {
                LOG_DEBUG("KEY_RF pressed to clear all notifications");
                setFocusItem(notifications);
                return notifications->clearAll(inputEvent);
            }
            if (inputEvent.is(gui::KeyCode::KEY_LF)) {
                LOG_DEBUG("KEY_LF pressed to navigate to calls");
                return app->showCalls();
            }
            return false;
        };
        return true;
    }

    app::ApplicationDesktop *DesktopMainWindow::getAppDesktop() const
    {
        auto *app = dynamic_cast<app::ApplicationDesktop *>(application);
        assert(app);
        return app;
    }

    auto DesktopMainWindow::getScreenLock() -> std::unique_ptr<PinLock>
    {
        auto lock = std::make_unique<PinLock>(getAppDesktop()->lockHandler.screenLock);
        auto app  = getAppDesktop();
        if (lock->getState() == PinLock::LockState::PasscodeRequired) {
            lock->onActivatedCallback = [app](const std::vector<unsigned int> &data) {
                app->lockHandler.handleScreenPin(data);
                app->switchWindow(app::window::name::desktop_main_window, ShowMode::GUI_SHOW_RETURN);
            };
        }
        else if (lock->getState() == PinLock::LockState::PasscodeInvalidRetryRequired) {
            lock->onActivatedCallback = [app](const std::vector<unsigned int> &) {
                app->lockHandler.screenLock.consumeState();
                app->switchWindow(app::window::name::desktop_main_window, ShowMode::GUI_SHOW_RETURN);
            };
        }
        else if (lock->getState() == PinLock::LockState::Blocked) {
            lock->onActivatedCallback = [app](const std::vector<unsigned int> &) {
                app->switchWindow(app::window::name::desktop_main_window, ShowMode::GUI_SHOW_INIT);
            };
        }
        return lock;
    }

    void DesktopMainWindow::switchToPinLockWindow(std::unique_ptr<PinLock> &&lock)
    {
        auto data = std::make_unique<LockPhoneData>(std::move(lock));
        if (!lockTimeoutApplilcation.empty()) {
            // if there was no application on to before closing proceed normally to pin protection
            data->setPrevApplication(lockTimeoutApplilcation);
            lockTimeoutApplilcation.clear();
        }
        application->switchWindow(app::window::name::desktop_pin_lock, std::move(data));
    }
} /* namespace gui */

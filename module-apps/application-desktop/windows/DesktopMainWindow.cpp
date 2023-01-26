// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "DesktopData.hpp"
#include "DesktopMainWindow.hpp"

#include <application-call/data/CallSwitchData.hpp>
#include <log/log.hpp>
#include <messages/DialogMetadataMessage.hpp>
#include <notifications/NotificationsModel.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-time/ServiceTime.hpp>
#include <service-time/TimeMessage.hpp>
#include <time/time_conversion_factory.hpp>
#include <windows/Dialog.hpp>
#include <windows/DialogMetadata.hpp>

#include <memory>

namespace gui
{
    void DesktopMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setActive(nav_bar::Side::Center, true);

        clockDate = new ClockDateWidget(
            this, 0, style::window::default_vertical_pos, style::window_width, clock_date_widget::h);

        activatedCallback = [this]([[maybe_unused]] Item &item) {
            if (notificationsModel->isTetheringOn()) {
                showInformationPopup(
                    [this]() {
                        app::manager::Controller::sendAction(application, app::manager::actions::Home);
                        return true;
                    },
                    utils::translate("tethering_menu_access_decline"));
            }
            else {
                application->switchWindow(app::window::name::desktop_menu);
            }
            return true;
        };

        notificationsList                    = new ListView(this,
                                         style::notifications::model::x,
                                         style::notifications::model::y,
                                         style::notifications::model::w,
                                         style::notifications::model::h,
                                         notificationsListPresenter,
                                         listview::ScrollBarType::Fixed);
        notificationsList->emptyListCallback = [this]() {
            setFocusItem(nullptr);
            setActiveState();
        };
        notificationsList->notEmptyListCallback = [this]() { setActiveState(); };
        setVisibleState();
    }

    void DesktopMainWindow::destroyInterface()
    {
        erase();
    }

    status_bar::Configuration DesktopMainWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.disable(status_bar::Indicator::Time);
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        appConfiguration.enable(status_bar::Indicator::Battery);
        appConfiguration.enable(status_bar::Indicator::Signal);
        appConfiguration.enable(status_bar::Indicator::SimCard);
        appConfiguration.enable(status_bar::Indicator::Bluetooth);
        appConfiguration.enable(status_bar::Indicator::AlarmClock);
        appConfiguration.enable(status_bar::Indicator::Tethering);
        return appConfiguration;
    }

    DesktopMainWindow::DesktopMainWindow(app::ApplicationCommon *app)
        : AppWindow(app, app::window::name::desktop_main_window),
          notificationsListPresenter(std::make_shared<ActiveNotificationsListPresenter>(this)),
          notificationsModel(std::make_shared<NotificationsModel>(notificationsListPresenter))
    {
        buildInterface();

        preBuildDrawListHook = [this](std::list<Command> &cmd) { updateTime(); };
    }

    void DesktopMainWindow::setVisibleState()
    {
        setActiveState();
    }

    void DesktopMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto notificationData = dynamic_cast<app::manager::actions::NotificationsChangedParams *>(data)) {
            notificationsModel->updateData(notificationData);
        }
        else {
            app::manager::Controller::requestNotifications(application);
        }

        setVisibleState();
    }

    bool DesktopMainWindow::processLongReleaseEvent(const InputEvent &inputEvent)
    {
        // long press of '0' key is translated to '+'
        if (inputEvent.is(KeyCode::KEY_0)) {
            return resolveDialAction(std::string{"+"});
        }
        if (inputEvent.is(KeyCode::KEY_RF)) {
            application->switchWindow(popup::window::power_off_window);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    namespace
    {
        constexpr auto pageFirstNotificationIdx = 0;
        constexpr auto pageLastNotificationIdx  = style::notifications::model::maxNotificationsPerPage - 1;
    } // namespace

    bool DesktopMainWindow::processShortReleaseEvent(const InputEvent &inputEvent)
    {
        auto code = translator.handle(inputEvent.getRawKey(), InputMode({InputMode::phone}).get());
        // if numeric key was pressed record that key and send it to call application
        if (code != 0) {
            const auto &number = std::string(1, static_cast<char>(code));
            return resolveDialAction(number);
        }
        else if (const auto notificationsNotFocused = (focusItem == nullptr);
                 notificationsNotFocused && !notificationsModel->isEmpty()) {
            if (inputEvent.is(KeyCode::KEY_UP)) {
                notificationsList->rebuildList(listview::RebuildType::OnPageElement, pageLastNotificationIdx);
                setFocusItem(notificationsList);
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_DOWN)) {
                notificationsList->rebuildList(listview::RebuildType::OnPageElement, pageFirstNotificationIdx);
                setFocusItem(notificationsList);
                return true;
            }
        }
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        if ((inputEvent.is(KeyCode::KEY_UP) || inputEvent.is(KeyCode::KEY_DOWN)) && focusItem != nullptr) {
            LOG_DEBUG("Notification box lost focus");
            setFocusItem(nullptr);
            setActiveState();
            return true;
        }
        return false;
    }

    bool DesktopMainWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isLongRelease()) {
            return processLongReleaseEvent(inputEvent);
        }
        else if (inputEvent.isShortRelease()) {
            return processShortReleaseEvent(inputEvent);
        }
        return AppWindow::onInput(inputEvent);
    }

    void DesktopMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void DesktopMainWindow::setActiveState()
    {
        if (focusItem != nullptr) {
            return;
        }
        navBar->setText(nav_bar::Side::Center, utils::translate("app_desktop_menu"));
        const auto tetheringNotActive = !notificationsModel->isTetheringOn();
        navBar->setText(nav_bar::Side::Left, utils::translate("app_desktop_calls"), tetheringNotActive);
        const auto hasDismissibleNotification = notificationsModel->hasDismissibleNotification();
        navBar->setText(nav_bar::Side::Right, utils::translate("app_desktop_clear_all"), hasDismissibleNotification);

        inputCallback = [this, hasDismissibleNotification, tetheringNotActive]([[maybe_unused]] Item &item,
                                                                               const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease() || notificationsList->focus) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_RF) && hasDismissibleNotification) {
                LOG_DEBUG("KEY_RF pressed to clear all notifications");
                notificationsModel->dismissAll();
                return true;
            }
            if (inputEvent.is(gui::KeyCode::KEY_LF) && tetheringNotActive) {
                LOG_DEBUG("KEY_LF pressed to navigate to calls");
                return app::manager::Controller::sendAction(application, app::manager::actions::ShowCallLog);
            }
            return false;
        };
    }

    app::ApplicationDesktop *DesktopMainWindow::getAppDesktop() const
    {
        auto *app = dynamic_cast<app::ApplicationDesktop *>(application);
        assert(app);
        return app;
    }

    RefreshModes DesktopMainWindow::updateTime()
    {
        AppWindow::updateTime();
        clockDate->setTime(std::time(nullptr));
        return RefreshModes::GUI_REFRESH_FAST;
    }

    bool DesktopMainWindow::showInformationPopup(std::function<bool()> action, const std::string &notification)
    {
        DialogMetadata meta;
        meta.icon       = "info_128px_W_G";
        meta.text       = notification;
        meta.action     = std::move(action);
        auto switchData = std::make_unique<DialogMetadataMessage>(std::move(meta));
        application->switchWindow(window::name::dialog_confirm, std::move(switchData));
        return true;
    }

    bool DesktopMainWindow::resolveDialAction(const std::string &number)
    {
        if (notificationsModel->isTetheringOn()) {
            return false;
        }
        else {
            return app::manager::Controller::sendAction(
                application, app::manager::actions::Dial, std::make_unique<app::EnterNumberData>(number));
        }
    }

} /* namespace gui */

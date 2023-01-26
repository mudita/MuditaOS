// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedWindow.hpp"
#include "PhoneLockedInfoData.hpp"

#include <locks/input/PhoneLockedKeysWhitelist.hpp>
#include <service-appmgr/Controller.hpp>
#include <popups/data/PopupData.hpp>
#include <notifications/NotificationsModel.hpp>

namespace gui
{
    PhoneLockedWindow::PhoneLockedWindow(app::ApplicationCommon *app,
                                         const std::string &name,
                                         std::unique_ptr<WallpaperPresenter> &&presenter,
                                         unsigned lockScreenDeepRefreshRate)
        : AppWindow(app, name), wallpaperPresenter(std::move(presenter))
    {
        buildInterface();
        initializeDeepRefreshCounter(lockScreenDeepRefreshRate);

        preBuildDrawListHook = [this](std::list<Command> &cmd) {
            AppWindow::updateTime();
            wallpaperPresenter->updateWallpaper();
        };
    }

    void PhoneLockedWindow::buildInterface()
    {
        AppWindow::buildInterface();
        wallpaperPresenter->setupWallpaper(this);
    }

    void PhoneLockedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto notificationsModel = wallpaperPresenter->getNotificationsModel();
        auto notificationData   = dynamic_cast<app::manager::actions::NotificationsChangedParams *>(data);
        if (notificationData) {
            notificationsModel->updateData(notificationData);
        }
        else {
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setActive(nav_bar::Side::Center, false);
            navBar->setActive(nav_bar::Side::Right, false);
            app::manager::Controller::requestNotifications(application);
            return;
        }

        if (notificationsModel->isPhoneTimeLock()) {
            wallpaperPresenter->forceClockWallpaper();
            if (notificationData) {
                notificationsModel->updateData(notificationData);
            }

            if (!refreshedOnPhoneLockTimeLock) {
                application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                refreshedOnPhoneLockTimeLock = true;
            }

            navBar->setText(nav_bar::Side::Left, utils::translate("app_desktop_emergency"));
            navBar->setActive(nav_bar::Side::Center, false);
            navBar->setActive(nav_bar::Side::Right, false);
        }
        else {
            if (wallpaperPresenter->switchBackWallpaper() && notificationData) {
                notificationsModel->updateData(notificationData);
            }
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setText(nav_bar::Side::Center, utils::translate("app_desktop_unlock"));
            navBar->setActive(nav_bar::Side::Right, false);
        }
    }

    RefreshModes PhoneLockedWindow::updateTime()
    {
        RefreshModes mode;

        std::bitset<2> refresh;
        refresh[0] = AppWindow::updateTime() == RefreshModes::GUI_REFRESH_FAST;
        refresh[1] = wallpaperPresenter->updateWallpaper();

        deepRefreshCounter++;

        if (refresh.none()) {
            LOG_DEBUG("Nothing to refresh on phone lock screen");
            return RefreshModes::GUI_REFRESH_NONE;
        }

        if (deepRefreshCounter.overflow()) {
            deepRefreshCounter.reset();
            mode = RefreshModes::GUI_REFRESH_DEEP;
        }
        else {
            mode = RefreshModes::GUI_REFRESH_FAST;
        }

        LOG_DEBUG("Requesting %s refresh on phone lock screen (%s)",
                  mode == RefreshModes::GUI_REFRESH_FAST ? "fast" : "deep",
                  refresh.to_string().c_str());
        return mode;
    }

    bool PhoneLockedWindow::processLongReleaseEvent(const InputEvent &inputEvent)
    {
        if (inputEvent.is(KeyCode::KEY_RF)) {

            application->switchWindow(gui::popup::window::power_off_window);
        }
        return true;
    }

    bool PhoneLockedWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (locks::PhoneLockedKeysWhitelist::isOnWhitelist(inputEvent)) {
            return AppWindow::onInput(inputEvent);
        }

        if (inputEvent.isLongRelease()) {
            return processLongReleaseEvent(inputEvent);
        }
        else if (inputEvent.isShortRelease() && navBar->isActive(nav_bar::Side::Center)) {
            const auto requiredStage = (inputEvent.is(KeyCode::KEY_ENTER)) ? PhoneLockedInfoData::Stage::Waiting
                                                                           : PhoneLockedInfoData::Stage::Idle;
            application->switchWindow(gui::popup::window::phone_lock_info_window,
                                      std::make_unique<PhoneLockedInfoData>(requiredStage));
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_LF) && navBar->isActive(nav_bar::Side::Left)) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
        }
        return true;
    }

    void PhoneLockedWindow::onClose(Window::CloseReason reason)
    {}

    status_bar::Configuration PhoneLockedWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.disable(status_bar::Indicator::Time);
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        appConfiguration.enable(status_bar::Indicator::Lock);
        appConfiguration.enable(status_bar::Indicator::Battery);
        appConfiguration.enable(status_bar::Indicator::Signal);
        appConfiguration.disable(status_bar::Indicator::SimCard);
        appConfiguration.enable(status_bar::Indicator::Bluetooth);
        appConfiguration.enable(status_bar::Indicator::AlarmClock);
        appConfiguration.enable(status_bar::Indicator::Tethering);

        return appConfiguration;
    }

    void PhoneLockedWindow::initializeDeepRefreshCounter(unsigned deepRefreshRate)
    {
        constexpr auto fallbackDeepRefreshRate = 30;
        deepRefreshCounter.reset(); // Clear the counter

        if (deepRefreshRate == 0) {
            LOG_ERROR("Invalid lock screen deep refresh value (%d) provided, fallback default (%d) loaded!",
                      deepRefreshRate,
                      fallbackDeepRefreshRate);
            deepRefreshCounter.setReference(fallbackDeepRefreshRate);
            return;
        }

        deepRefreshCounter.setReference(deepRefreshRate);
    }
} /* namespace gui */

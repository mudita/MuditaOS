// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedWindow.hpp"
#include "PhoneLockedInfoData.hpp"

#include <locks/input/PhoneLockedKeysWhitelist.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    PhoneLockedWindow::PhoneLockedWindow(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name),
          notificationsModel(std::make_shared<NotificationsModel>(NotificationsListPlacement::LockedScreen))
    {
        buildInterface();

        preBuildDrawListHook = [this](std::list<Command> &cmd) { updateTime(); };
    }

    void PhoneLockedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        clockDate = new ClockDateWidget(
            this, 0, style::window::default_vertical_pos, style::window_width, clock_date_widget::h);

        notificationsList = new ListView(this,
                                         style::notifications::model::x,
                                         style::notifications::model::y,
                                         style::notifications::model::w,
                                         style::notifications::model::h,
                                         notificationsModel,
                                         listview::ScrollBarType::None);
    }

    status_bar::Configuration PhoneLockedWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(status_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.disable(status_bar::Indicator::Time);
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        appConfiguration.enable(status_bar::Indicator::Lock);
        appConfiguration.enable(status_bar::Indicator::Battery);
        appConfiguration.enable(status_bar::Indicator::Signal);
        appConfiguration.enable(status_bar::Indicator::SimCard);
        appConfiguration.enable(status_bar::Indicator::Bluetooth);
        appConfiguration.enable(status_bar::Indicator::AlarmClock);

        return appConfiguration;
    }

    void PhoneLockedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto notificationData = dynamic_cast<app::manager::actions::NotificationsChangedParams *>(data);
        if (notificationData) {
            notificationsModel->updateData(notificationData);
        }
        else if (!notificationsModel->isPhoneTimeLock()) {
            app::manager::Controller::requestNotifications(application);
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setActive(nav_bar::Side::Center, false);
            navBar->setActive(nav_bar::Side::Right, false);
            return;
        }

        if (notificationsModel->isPhoneTimeLock()) {
            if (!refreshedOnPhoneLockTimeLock) {
                application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                refreshedOnPhoneLockTimeLock = true;
            }

            navBar->setText(nav_bar::Side::Left, utils::translate("app_desktop_emergency"));
            navBar->setActive(nav_bar::Side::Center, false);
            navBar->setActive(nav_bar::Side::Right, false);
        }
        else {
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setText(nav_bar::Side::Center, utils::translate("app_desktop_unlock"));
            navBar->setActive(nav_bar::Side::Right, false);
        }
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

    bool PhoneLockedWindow::updateTime()
    {
        auto ret = AppWindow::updateTime();
        clockDate->setTime(std::time(nullptr));
        return ret;
    }
} /* namespace gui */

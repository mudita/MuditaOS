// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedWindow.hpp"

#include <application-desktop/data/DesktopStyle.hpp>
#include <module-services/service-appmgr/service-appmgr/Controller.hpp>
#include <service-time/ServiceTime.hpp>
#include <service-time/TimeMessage.hpp>

namespace gui
{
    bool PhoneLockedWindow::LockingTimer::storeEnter(const InputEvent &evt)
    {
        enterPressed    = evt.is(KeyCode::KEY_ENTER);
        unLockStartTime = xTaskGetTickCount();
        return enterPressed;
    }

    void PhoneLockedWindow::LockingTimer::clear() noexcept
    {
        enterPressed = false;
    }

    bool PhoneLockedWindow::LockingTimer::cached() noexcept
    {
        if (xTaskGetTickCount() - unLockStartTime >= unLockTime) {
            clear();
        }
        return enterPressed;
    }

    PhoneLockedWindow::PhoneLockedWindow(app::Application *app, const std::string &name)
        : AppWindow(app, name), notificationsModel(std::make_shared<NotificationsModel>())
    {
        buildInterface();

        preBuildDrawListHook = [this](std::list<Command> &cmd) { updateTime(); };
    }

    void PhoneLockedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        using namespace style::desktop;

        time = new gui::Label(this, timeLabel::X, timeLabel::Y, timeLabel::Width, timeLabel::Height);
        time->setFilled(false);
        time->setBorderColor(gui::ColorNoColor);
        time->setFont(style::window::font::supersizemelight);
        time->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        dayText = new gui::Label(this, dayLabel::X, dayLabel::Y, dayLabel::Width, dayLabel::Height);
        dayText->setFilled(false);
        dayText->setBorderColor(gui::ColorNoColor);
        dayText->setFont(style::window::font::biglight);
        dayText->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

        notificationsList = new ListView(this,
                                         style::notifications::model::x,
                                         style::notifications::model::y,
                                         style::notifications::model::w,
                                         style::notifications::model::h,
                                         notificationsModel,
                                         listview::ScrollBarType::None);
    }

    top_bar::Configuration PhoneLockedWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.disable(top_bar::Indicator::Time);
        appConfiguration.enable(top_bar::Indicator::PhoneMode);
        appConfiguration.enable(top_bar::Indicator::Lock);
        appConfiguration.enable(top_bar::Indicator::Battery);
        appConfiguration.enable(top_bar::Indicator::Signal);
        appConfiguration.enable(top_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void PhoneLockedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto notificationData = dynamic_cast<app::manager::actions::NotificationsChangedParams *>(data)) {
            notificationsModel->updateData(notificationData);
        }
        else {
            app::manager::Controller::requestNotifications(application);
        }

        bottomBar->setActive(BottomBar::Side::RIGHT, false);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("app_desktop_unlock"));
        bottomBar->setActive(BottomBar::Side::LEFT, false);

        application->bus.sendUnicast(std::make_shared<TimersProcessingStopMessage>(), service::name::service_time);
    }

    bool PhoneLockedWindow::processLongReleaseEvent(const InputEvent &inputEvent)
    {
        if (inputEvent.is(KeyCode::KEY_RF)) {

            application->switchWindow(gui::popup::window::power_off_window);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool PhoneLockedWindow::processShortReleaseEvent(const InputEvent &inputEvent)
    {
        // if enter was pressed
        if (lockingTimer.cached() && inputEvent.is(KeyCode::KEY_PND)) {
            // if interval between enter and pnd keys is less than time defined for unlocking
            // display pin lock screen or simply refresh current window to update labels

            application->getPhoneLockSubject().unlock();
            return true;
        }
        else if (lockingTimer.storeEnter(inputEvent)) {
            return true;
        }
        // back not allowed on blocked screen
        else if (inputEvent.is(KeyCode::KEY_RF)) {
            application->switchWindow(gui::popup::window::phone_lock_info_window);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        else if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        application->switchWindow(gui::popup::window::phone_lock_info_window);

        return true;
    }

    bool PhoneLockedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isLongRelease()) {
            return processLongReleaseEvent(inputEvent);
        }
        else if (inputEvent.isShortRelease()) {
            return processShortReleaseEvent(inputEvent);
        }
        return AppWindow::onInput(inputEvent);
    }

    bool PhoneLockedWindow::updateTime()
    {
        using namespace utils::time;
        auto ret       = AppWindow::updateTime();
        auto timestamp = utils::time::getCurrentTimestamp();
        if (time != nullptr) {
            auto fmt = utils::dateAndTimeSettings.isTimeFormat12()
                           ? Locale::format(Locale::TimeFormat::FormatTime12HShort)
                           : Locale::format(Locale::TimeFormat::FormatTime24H);
            time->setText(timestamp.str(fmt));
        }
        if (dayText != nullptr) {
            dayText->setText(timestamp.str("%A, %d %b"));
        }
        return ret;
    }
} /* namespace gui */

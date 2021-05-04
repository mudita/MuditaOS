﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>

#include "DesktopMainWindow.hpp"
#include <application-desktop/ApplicationDesktop.hpp>
#include <application-desktop/data/DesktopStyle.hpp>
#include <application-desktop/data/DesktopData.hpp>
#include <application-call/data/CallSwitchData.hpp>

#include <service-appmgr/Controller.hpp>
#include <service-time/ServiceTime.hpp>
#include <service-time/TimeMessage.hpp>

#include <log/log.hpp>

namespace gui
{
    void DesktopMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);

        namespace timeLabel = style::desktop::timeLabel;
        namespace dayLabel  = style::desktop::dayLabel;

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

        activatedCallback = [this]([[maybe_unused]] Item &item) {
            application->switchWindow(app::window::name::desktop_menu);
            return true;
        };

        notificationsList                    = new ListView(this,
                                         style::notifications::model::x,
                                         style::notifications::model::y,
                                         style::notifications::model::w,
                                         style::notifications::model::h,
                                         notificationsModel,
                                         listview::ScrollBarType::Fixed);
        notificationsList->emptyListCallback = [this]() {
            setFocusItem(nullptr);
            setVisibleState();
        };
        notificationsList->notEmptyListCallback = [this]() {
            if (focusItem == nullptr) {
                setVisibleState();
            }
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
        time    = nullptr;
        dayText = nullptr;
    }

    top_bar::Configuration DesktopMainWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.enable(top_bar::Indicator::PhoneMode);
        return appConfiguration;
    }

    DesktopMainWindow::DesktopMainWindow(app::Application *app)
        : AppWindow(app, app::window::name::desktop_main_window),
          notificationsModel(std::make_shared<ActiveNotificationsModel>(this))
    {
        osUpdateVer  = getAppDesktop()->getOsUpdateVersion();
        osCurrentVer = getAppDesktop()->getOsCurrentVersion();

        buildInterface();

        preBuildDrawListHook = [this](std::list<Command> &cmd) { updateTime(); };
    }

    void DesktopMainWindow::setVisibleState()
    {
        auto app = getAppDesktop();

        app->lockHandler.unlockScreen();

        setActiveState();

        if (osUpdateVer == osCurrentVer && osUpdateVer != updateos::initSysVer &&
            osCurrentVer != updateos::initSysVer) {
            auto data = std::make_unique<CurrentOsVersion>();
            data->setData(osCurrentVer);
            application->switchWindow(app::window::name::desktop_post_update_window, std::move(data));
            getAppDesktop()->setOsUpdateVersion(updateos::initSysVer);
        }

        if (app->need_sim_select && Store::GSM::get()->sim == Store::GSM::SIM::SIM_UNKNOWN) {
            app::manager::Controller::sendAction(application, app::manager::actions::SelectSimCard);
        }

        application->bus.sendUnicast(std::make_shared<TimersProcessingStartMessage>(), service::name::service_time);
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
            return app::manager::Controller::sendAction(
                application, app::manager::actions::Dial, std::make_unique<app::EnterNumberData>("+"));
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
            return app::manager::Controller::sendAction(
                application, app::manager::actions::Dial, std::make_unique<app::EnterNumberData>(number));
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

    auto DesktopMainWindow::setActiveState() -> bool
    {
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate("app_desktop_menu"));
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate("app_desktop_calls"));
        const auto hasDismissibleNotification = notificationsModel->hasDismissibleNotification();
        bottomBar->setText(
            BottomBar::Side::RIGHT, utils::translate("app_desktop_clear_all"), hasDismissibleNotification);

        auto app      = getAppDesktop();
        inputCallback = [this, app, hasDismissibleNotification]([[maybe_unused]] Item &item,
                                                                const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease() || notificationsList->focus) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_RF) && hasDismissibleNotification) {
                LOG_DEBUG("KEY_RF pressed to clear all notifications");
                notificationsModel->dismissAll(inputEvent);
                return true;
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

    bool DesktopMainWindow::updateTime()
    {
        using namespace utils::time;
        auto ret       = AppWindow::updateTime();
        auto timestamp = utils::time::Timestamp();
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

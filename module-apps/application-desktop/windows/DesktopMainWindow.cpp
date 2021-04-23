// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>

#include "Alignment.hpp"
#include "BottomBar.hpp"
#include "DesktopMainWindow.hpp"
#include "GuiTimer.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-desktop/data/LockPhoneData.hpp"
#include "application-desktop/data/Style.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "gui/widgets/Image.hpp"
#include <service-appmgr/Controller.hpp>
#include <service-time/ServiceTime.hpp>
#include <service-time/TimeMessage.hpp>

#include <i18n/i18n.hpp>
#include "log/log.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <cassert>
#include <time/time_conversion.hpp>
#include <module-apps/application-call/data/CallSwitchData.hpp>

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

        applyToTopBar(
            [this](top_bar::Configuration configuration) { return configureTopBar(std::move(configuration)); });

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
    }

    top_bar::Configuration DesktopMainWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        auto app            = getAppDesktop();
        const auto isLocked = app->lockHandler.isScreenLocked();
        appConfiguration.setIndicator(top_bar::Indicator::Lock, isLocked);
        appConfiguration.disable(top_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.enable(top_bar::Indicator::PhoneMode);
        return appConfiguration;
    }

    DesktopMainWindow::DesktopMainWindow(app::Application *app, std::shared_ptr<NotificationsModel> model)
        : AppWindow(app, app::window::name::desktop_main_window), notificationsModel(std::move(model))
    {
        notificationsModel->setParentWindow(this);
        osUpdateVer  = getAppDesktop()->getOsUpdateVersion();
        osCurrentVer = getAppDesktop()->getOsCurrentVersion();

        buildInterface();

        preBuildDrawListHook = [this](std::list<Command> &cmd) { updateTime(); };
    }

    DesktopMainWindow::~DesktopMainWindow()
    {
        notificationsModel->clearAll();
        notificationsModel->list = nullptr;
    }

    void DesktopMainWindow::setVisibleState()
    {
        if (auto app = getAppDesktop(); app->lockHandler.isScreenLocked()) {
            bottomBar->setText(BottomBar::Side::CENTER, utils::translate("app_desktop_unlock"));
            bottomBar->setActive(BottomBar::Side::RIGHT, false);
            bottomBar->setText(
                BottomBar::Side::LEFT, utils::translate("app_desktop_emergency"), app->lockHandler.isScreenBlocked());

            inputCallback = nullptr;
            setFocusItem(nullptr);

            application->bus.sendUnicast(std::make_shared<TimersProcessingStopMessage>(), service::name::service_time);
        }
        else {
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
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void DesktopMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        app::manager::Controller::requestNotifications(application);
        setVisibleState();
    }

    bool DesktopMainWindow::processLongPressEvent(const InputEvent &inputEvent)
    {
        auto app = getAppDesktop();

        if (!app->lockHandler.isScreenLocked()) {
            if (inputEvent.is(KeyCode::KEY_PND)) {
                app->lockHandler.lockScreen();
                setVisibleState();
                return true;
            }
            // long press of '0' key is translated to '+'
            else if (inputEvent.is(KeyCode::KEY_0)) {
                return app::manager::Controller::sendAction(
                    application, app::manager::actions::Dial, std::make_unique<app::EnterNumberData>("+"));
            }
        }

        if (inputEvent.is(KeyCode::KEY_RF)) {
            application->switchWindow(app::window::name::desktop_poweroff);
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

    bool DesktopMainWindow::processShortPressEventOnUnlocked(const InputEvent &inputEvent)
    {
        auto code = translator.handle(inputEvent.key, InputMode({InputMode::phone}).get());
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

    bool DesktopMainWindow::processShortPressEventOnLocked(const InputEvent &inputEvent)
    {
        // if enter was pressed
        if (enter_cache.cached() && inputEvent.is(KeyCode::KEY_PND)) {
            // if interval between enter and pnd keys is less than time defined for unlocking
            // display pin lock screen or simply refresh current window to update labels

            getAppDesktop()->lockHandler.unlockScreen();
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
            app::manager::Controller::sendAction(application, app::manager::actions::ShowEmergencyContacts);
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
            if (!inputEvent.isShortPress() || notificationsList->focus) {
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

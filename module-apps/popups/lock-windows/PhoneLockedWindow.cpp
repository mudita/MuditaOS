// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>
#include <module-apps/application-desktop/data/DesktopStyle.hpp>
#include <module-utils/time/DateAndTimeSettings.hpp>
#include <module-services/service-appmgr/service-appmgr/Controller.hpp>

#include "Application.hpp"

#include "PhoneLockedWindow.hpp"

namespace gui
{
    PhoneLockedWindow::PhoneLockedWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
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
        // To be deleted?
        applyToTopBar(
            [this](top_bar::Configuration configuration) { return configureTopBar(std::move(configuration)); });

        bottomBar->setActive(BottomBar::Side::RIGHT, false);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
        bottomBar->setActive(BottomBar::Side::LEFT, false);

        // To be added
        // buildNotifications(app);

    }

    bool PhoneLockedWindow::processLongPressEvent(const InputEvent &inputEvent)
    {
        if (inputEvent.is(KeyCode::KEY_RF)) {

            // tutaj ma być chyba jeśli zakładamy żę tylko desktop to może -> do wyjasnienia
            //  application->switchWindow(app::window::name::desktop_poweroff);
            return true;
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool PhoneLockedWindow::processShortPressEventOnLocked(const InputEvent &inputEvent)
    {
        // if enter was pressed
        if (enter_cache.cached() && inputEvent.is(KeyCode::KEY_PND)) {
            // if interval between enter and pnd keys is less than time defined for unlocking
            // display pin lock screen or simply refresh current window to update labels

            LOG_ERROR("Chce dac unlocka");

            //            getAppDesktop()->lockHandler.unlockScreen();
            return true;
        }
        else if (enter_cache.storeEnter(inputEvent)) {
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
        if (inputEvent.isLongPress()) {
            return processLongPressEvent(inputEvent);
        }
        else if (inputEvent.isShortPress()) {
            return processShortPressEventOnLocked(inputEvent);
        }
        return AppWindow::onInput(inputEvent);
    }

    bool PhoneLockedWindow::updateTime()
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

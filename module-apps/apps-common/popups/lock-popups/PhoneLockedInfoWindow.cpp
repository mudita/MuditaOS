// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedInfoWindow.hpp"

#include <locks/input/PhoneLockedKeysWhitelist.hpp>
#include <service-appmgr/Controller.hpp>

#include <i18n/i18n.hpp>

using namespace gui;

PhoneLockedInfoWindow::PhoneLockedInfoWindow(app::ApplicationCommon *app, const std::string &name)
    : WindowWithTimer(app, name, std::chrono::seconds{4})
{
    buildInterface();
}

void PhoneLockedInfoWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
{
    auto info = dynamic_cast<PhoneLockedInfoData *>(data);

    mStage = (info && info->getStage() == PhoneLockedInfoData::Stage::Waiting) ? PhoneLockedInfoData::Stage::Waiting
                                                                               : PhoneLockedInfoData::Stage::Idle;

    if (mStage == PhoneLockedInfoData::Stage::Waiting) {
        navBar->setActive(nav_bar::Side::Left, true);
        navBar->setActive(nav_bar::Side::Center, false);
        navBar->setActive(nav_bar::Side::Right, true);
        infoIcon->text->setRichText(utils::translate("app_desktop_press_to_complete_unlock"));
    }
    else {
        navBar->setActive(nav_bar::Side::Left, true);
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        infoIcon->text->setRichText(utils::translate("app_desktop_press_to_unlock"));
    }

    WindowWithTimer::onBeforeShow(mode, data);
}

bool PhoneLockedInfoWindow::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    if (locks::PhoneLockedKeysWhitelist::isOnWhitelist(inputEvent)) {
        return AppWindow::onInput(inputEvent);
    }

    // Right key = go back
    if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
        detachTimerIfExists();
        application->returnToPreviousWindow();
        return true;
    }

    // Left key = SOS call, it should work all the time
    else if (inputEvent.isShortRelease(KeyCode::KEY_LF) && navBar->isActive(nav_bar::Side::Left)) {
        app::manager::Controller::sendAction(application,
                                             app::manager::actions::EmergencyDial,
                                             std::make_unique<SwitchData>(),
                                             app::manager::OnSwitchBehaviour::RunInBackground);
        return true;
    }

    // Pnd key = go to PIN code screen
    else if (inputEvent.isShortRelease(KeyCode::KEY_PND) && mStage == PhoneLockedInfoData::Stage::Waiting) {
        detachTimerIfExists();
        application->getPhoneLockSubject().unlock();
        return true;
    }

    // Enter key = start unlocking if we are in STARTING stage
    else if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) && mStage == PhoneLockedInfoData::Stage::Idle) {
        mStage = PhoneLockedInfoData::Stage::Waiting;
        navBar->setActive(nav_bar::Side::Center, false);
        infoIcon->text->setRichText(utils::translate("app_desktop_press_to_complete_unlock"));
        resetTimer();
        return true;
    }

    // Any other key - reset timers and go to STARTING stage
    else if (inputEvent.isShortRelease() && mStage == PhoneLockedInfoData::Stage::Waiting) {
        mStage = PhoneLockedInfoData::Stage::Idle;
        navBar->setActive(nav_bar::Side::Center, true);
        infoIcon->text->setRichText(utils::translate("app_desktop_press_to_unlock"));
        resetTimer();
        return true;
    }

    return WindowWithTimer::onInput(inputEvent);
}

status_bar::Configuration PhoneLockedInfoWindow::configureStatusBar(status_bar::Configuration appConfiguration)
{
    appConfiguration.disable(status_bar::Indicator::NetworkAccessTechnology);
    appConfiguration.disable(status_bar::Indicator::Time);
    appConfiguration.disable(status_bar::Indicator::SimCard);
    appConfiguration.enable(status_bar::Indicator::PhoneMode);
    appConfiguration.enable(status_bar::Indicator::Lock);
    appConfiguration.enable(status_bar::Indicator::Battery);
    appConfiguration.enable(status_bar::Indicator::Signal);
    appConfiguration.enable(status_bar::Indicator::Bluetooth);
    appConfiguration.enable(status_bar::Indicator::AlarmClock);
    return appConfiguration;
}

void PhoneLockedInfoWindow::buildInterface()
{
    WindowWithTimer::buildInterface();

    navBar->setText(nav_bar::Side::Left, utils::translate("app_desktop_emergency"));
    navBar->setText(nav_bar::Side::Center, utils::translate("app_desktop_unlock"));
    navBar->setText(nav_bar::Side::Right, utils::translate("common_back"));

    infoIcon = new gui::Icon(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height,
                             "lock_128px_W_G",
                             utils::translate("app_desktop_press_to_unlock"));

    infoIcon->setAlignment(Alignment::Horizontal::Center);
}

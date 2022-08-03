// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimPINSettingsPresenter.hpp"

#include <application-settings/data/PINSettingsLockStateData.hpp>
#include <application-settings/data/PINSettingsSimData.hpp>
#include <EventStore.hpp>

SimPINSettingsPresenter::SimPINSettingsPresenter(app::ApplicationCommon *application) : application(application)
{}

void SimPINSettingsPresenter::setCurrentPinState(bool state) noexcept
{
    isPinOn = state;
}

void SimPINSettingsPresenter::togglePinState()
{
    isPinOn = !isPinOn;
    if (!isPinOn) {
        application->getSimLockSubject().disableSimPin();
    }
    else {
        application->getSimLockSubject().enableSimPin();
    }
}

bool SimPINSettingsPresenter::isPinEnabled() const noexcept
{
    return isPinOn;
}

void SimPINSettingsPresenter::requestLockState() const
{
    application->bus.sendUnicast<cellular::msg::request::sim::GetLockState>();
}

void SimPINSettingsPresenter::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
{
    const auto view = getView();

    if (const auto pinSettingsSimData = dynamic_cast<gui::PINSettingsSimData *>(data); pinSettingsSimData != nullptr) {
        view->setTitle(utils::translate("app_settings_network_pin_settings") + " (" + pinSettingsSimData->getSim() +
                       ")");
    }

    if (const auto pinSettingsLockStateData = dynamic_cast<gui::PINSettingsLockStateData *>(data);
        pinSettingsLockStateData != nullptr) {
        view->setNavbarCenterActive(true);
        pinLockStateChanged = isPinEnabled() == pinSettingsLockStateData->getSimCardPinLockState() ? false : true;
        if (pinLockStateChanged) {
            const auto currentPinState = pinSettingsLockStateData->getSimCardPinLockState();
            setCurrentPinState(currentPinState);
        }
    }
    if (not Store::GSM::get()->simCardInserted()) {
        view->setNavbarCenterActive(false);
        return;
    }
    if (mode == gui::ShowMode::GUI_SHOW_RETURN) {
        requestLockState();
    }
    if (mode == gui::ShowMode::GUI_SHOW_INIT || pinLockStateChanged) {
        view->refreshOptionsList();
    }
}

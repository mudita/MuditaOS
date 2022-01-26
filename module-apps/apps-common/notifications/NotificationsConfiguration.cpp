// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsConfiguration.hpp"
#include <Utils.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

using namespace notifications;

NotificationsConfiguration::NotificationsConfiguration(std::shared_ptr<sys::phone_modes::Observer> phoneModeObserver,
                                                       std::shared_ptr<settings::Settings> settings,
                                                       const locks::PhoneLockHandler &phoneLockHandler)
    : phoneModeObserver{phoneModeObserver}, settings{settings}, phoneLockHandler{phoneLockHandler}
{}

void NotificationsConfiguration::updateCurrentCall(CallNotificationPolicy &policy)
{
    policy.updateCurrentCall(phoneModeObserver->getCurrentPhoneMode());
}

void NotificationsConfiguration::updateCurrentList(NotificationsListPolicy &policy)
{
    policy.updateCurrentList(phoneModeObserver->getCurrentPhoneMode(),
                             phoneLockHandler.isPhoneLocked(),
                             getDNDNotificationsOnLockedScreen());
}

void NotificationsConfiguration::updateCurrentSMS(SMSNotificationPolicy &policy)
{
    policy.updateCurrentSMS(phoneModeObserver->getCurrentPhoneMode());
}

auto NotificationsConfiguration::getDNDNotificationsOnLockedScreen() const noexcept -> bool
{
    return utils::getNumericValue<bool>(
        settings->getValue(settings::Offline::notificationsWhenLocked, settings::SettingsScope::Global));
}

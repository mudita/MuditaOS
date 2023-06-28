// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSNotificationPolicy.hpp"

using namespace notifications;

void SMSNotificationPolicy::updateCurrentSMS(sys::phone_modes::PhoneMode phoneMode)
{
    ringtoneOn = false;
    switch (phoneMode) {
    case sys::phone_modes::PhoneMode::DoNotDisturb:
        ringtoneOn = false;
        break;
    case sys::phone_modes::PhoneMode::Uninitialized:
    case sys::phone_modes::PhoneMode::Connected:
    case sys::phone_modes::PhoneMode::Offline:
        ringtoneOn = true;
        break;
    }
}

bool SMSNotificationPolicy::isRingtoneAllowed() const noexcept
{
    return ringtoneOn;
}

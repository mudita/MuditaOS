// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallNotificationPolicy.hpp"

using namespace notifications;

void CallNotificationPolicy::updateCurrentCall(sys::phone_modes::PhoneMode phoneMode)
{
    popupOn           = false;
    ringtoneOn        = false;
    numberCheckNeeded = false;
    dismissedCallNotification = false;

    switch (phoneMode) {
    case sys::phone_modes::PhoneMode::Connected:
        popupOn    = true;
        ringtoneOn = true;
        break;
    case sys::phone_modes::PhoneMode::DoNotDisturb:
        dismissedCallNotification = true;
        numberCheckNeeded = true;
        break;
    case sys::phone_modes::PhoneMode::Offline:
        dismissedCallNotification = false;
        break;
    }
}

bool CallNotificationPolicy::isPopupAllowed() const noexcept
{
    return popupOn && !numberCheckNeeded;
}

bool CallNotificationPolicy::isRingtoneAllowed() const noexcept
{
    return ringtoneOn;
}

bool CallNotificationPolicy::isDismissedCallNotificationAllowed() const noexcept
{
    return dismissedCallNotification;
}

bool CallNotificationPolicy::isNumberCheckRequired() const noexcept
{
    return numberCheckNeeded;
}

void CallNotificationPolicy::numberCheck(bool callsFromFavouritesSetting, bool isNumberinFavourites)
{
    numberCheckNeeded = false;
    popupOn           = callsFromFavouritesSetting && isNumberinFavourites;
    ringtoneOn        = callsFromFavouritesSetting && isNumberinFavourites;
}

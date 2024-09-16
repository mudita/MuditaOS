// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotificationsListPolicy.hpp"

using namespace notifications;

void NotificationsListPolicy::updateCurrentList(sys::phone_modes::PhoneMode phoneMode,
                                                bool isLocked,
                                                bool lockedScreenNotificationSetting)
{
    updateAllowed  = true;
    showWhenLocked = true;

    switch (phoneMode) {
    case sys::phone_modes::PhoneMode::DoNotDisturb:
        showWhenLocked = lockedScreenNotificationSetting;
        updateAllowed  = (isLocked && lockedScreenNotificationSetting) || (!isLocked);
        break;
    case sys::phone_modes::PhoneMode::Uninitialized:
    case sys::phone_modes::PhoneMode::Connected:
    case sys::phone_modes::PhoneMode::Offline:
        updateAllowed  = true;
        showWhenLocked = true;
        break;
    }
}

bool NotificationsListPolicy::updateListAllowed() const noexcept
{
    return updateAllowed;
}

bool NotificationsListPolicy::showListWhenLocked() const noexcept
{
    return showWhenLocked;
}

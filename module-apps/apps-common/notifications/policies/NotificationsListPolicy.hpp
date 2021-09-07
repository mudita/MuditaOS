// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>
#include <PhoneModes/Common.hpp>

namespace notifications
{
    class NotificationsListPolicy
    {
      public:
        void updateCurrentList(sys::phone_modes::PhoneMode phoneMode,
                               bool isLocked,
                               bool lockedScreenNotificationSetting);
        bool updateListAllowed() const noexcept;
        bool showListWhenLocked() const noexcept;

      private:
        bool showWhenLocked = true;
        bool updateAllowed  = true;
    };
} // namespace notifications

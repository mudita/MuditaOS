// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>
#include <PhoneModes/Common.hpp>

namespace notifications
{
    class CallNotificationPolicy
    {
      public:
        void updateCurrentCall(sys::phone_modes::PhoneMode phoneMode);
        bool isPopupAllowed() const noexcept;
        bool isRingtoneAllowed() const noexcept;
        bool isDismissedCallNotificationAllowed() const noexcept;
        void numberCheck(bool callsFromFavouritesSetting, bool isNumberInFavourites);

      private:
        bool popupOn;
        bool ringtoneOn;
        bool dismissedCallNotification;
        bool numberCheckNeeded;
    };
} // namespace notifications

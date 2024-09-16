// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <PhoneModes/Common.hpp>

namespace notifications
{
    class SMSNotificationPolicy
    {
      public:
        void updateCurrentSMS(sys::phone_modes::PhoneMode phoneMode);
        bool isRingtoneAllowed() const noexcept;

      private:
        bool ringtoneOn;
    };
} // namespace notifications

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

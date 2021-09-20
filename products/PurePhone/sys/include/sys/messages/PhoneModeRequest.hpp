// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneModes/Common.hpp>

namespace sys
{
    class PhoneModeRequest : public sys::DataMessage
    {
      public:
        explicit PhoneModeRequest(phone_modes::PhoneMode mode)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), mode{mode}
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept -> phone_modes::PhoneMode
        {
            return mode;
        }

      private:
        phone_modes::PhoneMode mode;
    };
} // namespace sys

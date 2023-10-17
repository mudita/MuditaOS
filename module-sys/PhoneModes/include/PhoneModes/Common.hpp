// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include "PhoneMode.hpp"
#include "Tethering.hpp"

namespace sys::phone_modes
{
    class PhoneModeChanged : public DataMessage
    {
      public:
        explicit PhoneModeChanged(PhoneMode phoneMode)
            : DataMessage{MessageType::MessageTypeUninitialized}, phoneMode{phoneMode}
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept
        {
            return phoneMode;
        }

      private:
        PhoneMode phoneMode;
    };

    class TetheringChanged : public DataMessage
    {
      public:
        explicit TetheringChanged(Tethering tetheringMode)
            : DataMessage{MessageType::MessageTypeUninitialized}, tethering{tetheringMode}
        {}

        [[nodiscard]] auto getTetheringMode() const noexcept
        {
            return tethering;
        }

      private:
        Tethering tethering;
    };

    class ChangedSuccessfully : public ResponseMessage
    {};

    class ChangeFailed : public ResponseMessage
    {};
} // namespace sys::phone_modes

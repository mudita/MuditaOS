// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneModes/Common.hpp>

namespace sys
{
    class TetheringStateRequest : public sys::DataMessage
    {
      public:
        explicit TetheringStateRequest(phone_modes::Tethering state)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), tetheringState{state}
        {}

        [[nodiscard]] auto getTetheringState() const noexcept -> phone_modes::Tethering
        {
            return tetheringState;
        }

      private:
        phone_modes::Tethering tetheringState;
    };
} // namespace sys

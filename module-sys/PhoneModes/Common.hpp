// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

namespace sys::phone_modes
{
    enum class PhoneMode
    {
        Connected,
        DoNotDisturb,
        Offline
    };

    enum class Tethering
    {
        Off,
        On
    };

    class PhoneModeChanged : public DataMessage
    {
      public:
        PhoneModeChanged(PhoneMode phoneMode, Tethering tetheringMode)
            : DataMessage{MessageType::MessageTypeUninitialized}, phoneMode{phoneMode}, tethering{tetheringMode}
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept
        {
            return phoneMode;
        }
        [[nodiscard]] auto getTetheringMode() const noexcept
        {
            return tethering;
        }

      private:
        PhoneMode phoneMode;
        Tethering tethering;
    };

    class PhoneModeChangedSuccessfully : public ResponseMessage
    {};

    class PhoneModeChangeFailed : public ResponseMessage
    {};
} // namespace sys::phone_modes

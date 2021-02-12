// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"

namespace sys
{
    class Service; // Forward declaration
} // namespace sys

namespace sys::phone_modes
{
    class Subject
    {
      public:
        explicit Subject(Service *owner);

        void setMode(PhoneMode _phoneMode, Tethering _tetheringMode);
        void setPhoneMode(PhoneMode mode);
        void setTetheringMode(Tethering mode);

      private:
        void notifyChange();
        bool changePhoneMode(PhoneMode mode) noexcept;
        bool changeTetheringMode(Tethering mode) noexcept;

        Service *owner;
        PhoneMode phoneMode     = PhoneMode::Connected;
        Tethering tetheringMode = Tethering::Off;
    };
} // namespace sys::phone_modes

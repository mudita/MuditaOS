// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <stdexcept>

#include "Common.hpp"

namespace sys
{
    class Service; // Forward declaration
} // namespace sys

namespace sys::phone_modes
{
    class Observer
    {
      public:
        using OnPhoneModeChangedCallback = std::function<void(PhoneMode)>;
        using OnTetheringChangedCallback = std::function<void(Tethering)>;

        void connect(Service *owner);
        void subscribe(OnPhoneModeChangedCallback &&onChange) noexcept;
        void subscribe(OnTetheringChangedCallback &&onChange) noexcept;

        bool isInMode(PhoneMode mode) const noexcept;
        PhoneMode getCurrentPhoneMode() const noexcept;
        bool isTetheringOn() const noexcept;

      private:
        sys::MessagePointer handlePhoneModeChange(PhoneModeChanged *message);
        sys::MessagePointer handleTetheringChange(TetheringChanged *message);

        OnPhoneModeChangedCallback onPhoneModeChangedCallback;
        OnTetheringChangedCallback onTetheringChangedCallback;
        PhoneMode phoneMode     = PhoneMode::Connected;
        Tethering tetheringMode = Tethering::Off;
    };
} // namespace sys::phone_modes

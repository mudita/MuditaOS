// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        using OnChangeCallback   = std::function<void(PhoneMode, Tethering)>;
        using OnCompleteCallback = std::function<void()>;
        using OnErrorCallback    = std::function<void(const std::exception &)>;

        void connect(Service *owner);
        void subscribe(OnChangeCallback &&onChange,
                       OnCompleteCallback &&onComplete = {},
                       OnErrorCallback &&onError       = {}) noexcept;

        bool isInMode(PhoneMode mode) const noexcept;
        PhoneMode getCurrentPhoneMode() const noexcept;
        bool isTetheringOn() const noexcept;

      private:
        sys::MessagePointer handlePhoneModeChange(PhoneModeChanged *message);
        void onPhoneModeChanged();

        OnChangeCallback onChangeCallback;
        OnCompleteCallback onCompleteCallback;
        OnErrorCallback onErrorCallback;
        PhoneMode phoneMode     = PhoneMode::Connected;
        Tethering tetheringMode = Tethering::Off;
    };
} // namespace sys::phone_modes

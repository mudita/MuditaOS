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
        using OnPhoneModeChangedCallback = std::function<void(PhoneMode)>;
        using OnTetheringChangedCallback = std::function<void(Tethering)>;
        using OnCompleteCallback         = std::function<void()>;
        using OnErrorCallback            = std::function<void(const std::exception &)>;
        struct OnFinishedCallbacks
        {
            OnCompleteCallback onComplete;
            OnErrorCallback onError;
        };

        void connect(Service *owner);
        void subscribe(OnPhoneModeChangedCallback &&onChange,
                       OnCompleteCallback &&onComplete = {},
                       OnErrorCallback &&onError       = {}) noexcept;
        void subscribe(OnTetheringChangedCallback &&onChange,
                       OnCompleteCallback &&onComplete = {},
                       OnErrorCallback &&onError       = {}) noexcept;

        bool isInMode(PhoneMode mode) const noexcept;
        PhoneMode getCurrentPhoneMode() const noexcept;
        bool isTetheringOn() const noexcept;

      private:
        sys::MessagePointer handlePhoneModeChange(PhoneModeChanged *message);
        sys::MessagePointer handleTetheringChange(TetheringChanged *message);

        OnPhoneModeChangedCallback onPhoneModeChangedCallback;
        OnTetheringChangedCallback onTetheringChangedCallback;
        OnFinishedCallbacks onPhoneModeChangeFinished;
        OnFinishedCallbacks onTetheringChangeFinished;
        PhoneMode phoneMode     = PhoneMode::Connected;
        Tethering tetheringMode = Tethering::Off;
    };
} // namespace sys::phone_modes

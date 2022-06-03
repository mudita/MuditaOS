// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include <functional>

namespace sys
{
    class Service; // Forward declaration
} // namespace sys

namespace sys::phone_modes
{
    class Subject
    {
      public:
        Subject(Service *owner, std::function<bool()> simSelect, std::function<bool()> isCallOngoing);

        /**
         * Sets phone and tethering modes
         * @param _phoneMode        Phone mode
         * @param _tetheringMode    Tethering mode
         * @return true if modes changed, false otherwise
         */
        bool setMode(PhoneMode _phoneMode, Tethering _tetheringMode);

        /**
         * Sets the phone mode
         * @param mode  Phone mode
         * @return true if phone mode changed, false otherwise
         */
        bool setPhoneMode(PhoneMode mode);

        /**
         * Sets the tethering mode
         * @param mode  Tethering mode
         * @return true if tethering mode changed, false otherwise
         */
        bool setTetheringMode(Tethering mode);

        bool isTetheringEnabled() const noexcept;

        bool isTetheringPossible() const noexcept;

      private:
        bool changePhoneMode(PhoneMode mode) noexcept;
        void notifyPhoneModeChange();
        bool changeTetheringMode(Tethering mode) noexcept;
        void notifyTetheringChange();

        Service *owner;
        PhoneMode phoneMode     = PhoneMode::Connected;
        Tethering tetheringMode = Tethering::Off;
        const std::function<bool()> simSelected;
        const std::function<bool()> isCallOngoing;
    };
} // namespace sys::phone_modes

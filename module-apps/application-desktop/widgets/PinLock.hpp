// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PhoneNumber.hpp"

namespace gui
{
    class PinLockHandler;

    class PinLock
    {
      public:
        enum class LockType
        {
            Screen,
            SIM,
            PUK,
            Unknown
        };
        enum class InfoName
        {
            LockName,
            PhoneNum
        };
        enum class State
        {
            EnterPin,
            InvalidPin,
            VerifiedPin,
            Blocked,
            Unlocked
        };

        [[nodiscard]] State getState() const noexcept
        {
            return state;
        }
        [[nodiscard]] unsigned int getMaxPinSize() const noexcept
        {
            return maxPinSize;
        }
        /// returns current position of a PIN character to be inserted
        [[nodiscard]] unsigned int getCharCount() const noexcept
        {
            return pinValue.size();
        }
        [[nodiscard]] unsigned int getRemainingAttempts() const noexcept
        {
            return remainingAttempts;
        }
        [[nodiscard]] bool canPut() const noexcept
        {
            return getCharCount() != getMaxPinSize();
        }
        [[nodiscard]] bool canVerify() const noexcept
        {
            return getCharCount() >= minPinSize;
        }
        void putNextChar(unsigned int c);
        void verifyPin();
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar();
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// if Lock is in the State::InvalidPin state, changes it's state to the State::EnterPin
        void consumeInvalidPinState() noexcept;

        [[nodiscard]] bool isLocked() const noexcept;
        bool unlock() noexcept;
        void lock() noexcept;

        [[nodiscard]] std::string getLockInfo(const InfoName name) const;
        [[nodiscard]] LockType getLockType() const noexcept
        {
            return type;
        }
        PinLock(gui::PinLockHandler *);

      private:
        /// for PIN verification purposes as PIN storage and management is out of scope of PinLock class
        gui::PinLockHandler *handler;

        LockType type                  = LockType::Unknown;
        State state                    = State::EnterPin;
        unsigned int remainingAttempts = 0;
        /// code of the entered character on specified position
        std::vector<unsigned int> pinValue;
        unsigned int maxPinSize = 0;
        unsigned int minPinSize = 0;
        std::map<InfoName, std::string> additionalLockInfo;

        void reset(LockType _type,
                   State _state,
                   unsigned int _remainingAttempts,
                   unsigned int _maxPinSize,
                   unsigned int _minPinSize) noexcept;

        friend class gui::PinLockHandler;
    };

} // namespace gui

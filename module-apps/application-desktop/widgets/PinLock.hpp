// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <vector>
#include <string>
#include <functional>

namespace gui
{
    class PinLockHandler;

    class PinLock
    {
      public:
        enum class LockType
        {
            SimPin,
            SimPuk,
            Screen
        };
        enum class LockState
        {
            Unlocked,
            PasscodeRequired,
            PasscodeInvalidRetryRequired,
            Blocked,
            NewPasscodeRequired,
            NewPasscodeInvalid,
            ErrorOccurred
        };
        enum class SimCard
        {
            SIM1,
            SIM2,
            NoCard
        };

        [[nodiscard]] LockState getState() const noexcept
        {
            return lockState;
        }
        [[nodiscard]] LockType getLockType() const noexcept
        {
            return lockType;
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
        [[nodiscard]] bool canPut() const noexcept
        {
            return getCharCount() < getMaxPinSize();
        }
        [[nodiscard]] bool canVerify() const noexcept
        {
            return getCharCount() >= minPinSize;
        }
        [[nodiscard]] std::vector<unsigned int> getPin() const
        {
            return pinValue;
        }
        [[nodiscard]] unsigned int getValue() const noexcept
        {
            return value;
        }

        void putNextChar(unsigned int c);
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar();
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// consumes LockState::PasscodeInvalidRetryRequired state and LockState::NewPasscodeInvalid)
        void consumeState() noexcept;
        void setNewPasscodeInvalidState() noexcept;

        void verify();

        PinLock(SimCard sim, LockState state, LockType type, unsigned int value)
            : sim{sim}, lockState{state}, lockType{type}, value{value}
        {}
        PinLock(const PinLock &other) = default;

        std::function<void(const std::vector<unsigned int> &)> onActivatedCallback = nullptr;

      private:
        SimCard sim         = SimCard::NoCard;
        LockState lockState = LockState::Unlocked;
        LockType lockType   = LockType::Screen;
        unsigned int value  = 0;

        std::vector<unsigned int> pinValue;
        unsigned int maxPinSize = defaultPasscodeSize;
        unsigned int minPinSize = defaultPasscodeSize;
        bool autoActivate       = false;

        static constexpr unsigned int defaultPasscodeSize = 4;

        void setAutoActivate(bool _autoActivate)
        {
            autoActivate = _autoActivate;
        }
        void setPinSizeBounds(unsigned int _minPinSize, unsigned int _maxPinSize)
        {
            minPinSize = _minPinSize;
            maxPinSize = _maxPinSize;
        }

        friend class PinLockHandler;
    };

} // namespace gui

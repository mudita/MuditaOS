// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <vector>
#include <string>
#include <functional>
#include <limits>

namespace lock
{
    class PhoneLockObserver;

    class Lock
    {
      public:
        enum class LockState
        {
            Unlocked,
            InputRequired,
            InvalidInputRetryRequired,
            Blocked,
        };

        [[nodiscard]] LockState getState() const noexcept
        {
            return lockState;
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

        [[nodiscard]] bool isState(LockState state) const noexcept
        {
            return lockState == state;
        }

        [[nodiscard]] const std::string &getPasscodeName() const noexcept
        {
            return passcodeName;
        }

        void putNextChar(unsigned int c);
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar();
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// consumes LockState::PasscodeInvalidRetryRequired state and LockState::NewPasscodeInvalid)
        void consumeState() noexcept;
        void setNewPasscodeInvalidState() noexcept;
        /// calls
        void activate();

        Lock(LockState state, unsigned int value = unlimitedNumOfAttempts) : lockState{state}, value{value}
        {}
        Lock(const Lock &other) = default;

        std::function<void(const std::vector<unsigned int> &)> onActivatedCallback = nullptr;

        // Clean that up

      private:
        std::string passcodeName;
        LockState lockState = LockState::Unlocked;
        unsigned int value  = 0;

        std::vector<unsigned int> pinValue;
        unsigned int maxPinSize = defaultPasscodeSize;
        unsigned int minPinSize = defaultPasscodeSize;
        bool autoActivate       = false;

        static constexpr unsigned int defaultPasscodeSize    = 4;
        static constexpr unsigned int unlimitedNumOfAttempts = std::numeric_limits<unsigned int>::max();

        void setAutoActivate(bool _autoActivate)
        {
            autoActivate = _autoActivate;
        }
        void setPinSizeBounds(unsigned int _minPinSize, unsigned int _maxPinSize)
        {
            minPinSize = _minPinSize;
            maxPinSize = _maxPinSize;
        }

        friend class PhoneLockObserver;
    };

} // namespace gui

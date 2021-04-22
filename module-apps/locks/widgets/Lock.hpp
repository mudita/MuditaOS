// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <vector>
#include <string>
#include <functional>
#include <limits>

#include <module-utils/common_data/EventStore.hpp>

namespace gui
{
    class PinLockHandler;
    class ChangePasscodeLockHandler;
} // namespace gui

namespace lock
{
    class PhoneLockHandler;

    class Lock
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
            InputRequired,
            InputInvalidRetryRequired,
            Blocked,
            NewInputRequired,
            NewInputInvalidRetryRequired,
            NewInputConfirmRequired,
            NewInputInvalid,
            ErrorOccurred
        };

        [[nodiscard]] LockState getState() const noexcept
        {
            return lockState;
        }
        [[nodiscard]] LockType getLockType() const noexcept
        {
            return lockType;
        }
        [[nodiscard]] unsigned int getMaxInputSize() const noexcept
        {
            return maxInputSize;
        }
        /// returns current position of a Input character to be inserted
        [[nodiscard]] unsigned int getCharCount() const noexcept
        {
            return inputValue.size();
        }
        [[nodiscard]] bool canPut() const noexcept
        {
            return getCharCount() < getMaxInputSize();
        }
        [[nodiscard]] bool canVerify() const noexcept
        {
            return getCharCount() >= minInputSize;
        }
        [[nodiscard]] std::vector<unsigned int> getInput() const
        {
            return inputValue;
        }
        [[nodiscard]] unsigned int getAttemptsLeft() const noexcept
        {
            return attemptsLeft;
        }
        [[nodiscard]] bool isSim(Store::GSM::SIM _sim) const noexcept
        {
            return sim == _sim;
        }
        [[nodiscard]] bool isState(LockState state) const noexcept
        {
            return lockState == state;
        }
        [[nodiscard]] bool isType(LockType type) const noexcept
        {
            return lockType == type;
        }
        [[nodiscard]] const std::string &getLockName() const noexcept
        {
            return lockName;
        }
        [[nodiscard]] Store::GSM::SIM getSim() const noexcept
        {
            return sim;
        }

        void putNextChar(unsigned int c);
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar();
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// consumes LockState::PasscodeInvalidRetryRequired state and LockState::NewPasscodeInvalid)
        void consumeState() noexcept;
        /// calls
        void activate();

        Lock(Store::GSM::SIM sim, LockState state, LockType type, unsigned int attemptsLeft = unlimitedNumOfAttempts)
            : sim{sim}, lockState{state}, lockType{type}, attemptsLeft{attemptsLeft}
        {}

        Lock(LockState state, unsigned int attemptsLeft = unlimitedNumOfAttempts)
            : lockState{state}, attemptsLeft{attemptsLeft}
        {}

        std::function<void(LockType type, const std::vector<unsigned int> &)> onActivatedCallback = nullptr;

      private:
        std::string lockName;
        Store::GSM::SIM sim       = Store::GSM::SIM::NONE;
        LockState lockState       = LockState::Unlocked;
        LockType lockType         = LockType::Screen;
        unsigned int attemptsLeft = 0;

        std::vector<unsigned int> inputValue;
        unsigned int maxInputSize = defaultInputSize;
        unsigned int minInputSize = defaultInputSize;
        bool autoActivate         = false;

        static constexpr unsigned int defaultInputSize       = 4;
        static constexpr unsigned int unlimitedNumOfAttempts = std::numeric_limits<unsigned int>::max();

        void setAutoActivate(bool _autoActivate)
        {
            autoActivate = _autoActivate;
        }
        void setInputSizeBounds(unsigned int _minInputSize, unsigned int _maxInputSize)
        {
            minInputSize = _minInputSize;
            maxInputSize = _maxInputSize;
        }

        friend class PhoneLockHandler;
        friend class gui::PinLockHandler;
        friend class gui::ChangePasscodeLockHandler;
    };

} // namespace lock

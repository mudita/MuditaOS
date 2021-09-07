// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <vector>
#include <string>
#include <functional>
#include <limits>

namespace locks
{
    class PhoneLockHandler;
    class SimLockHandler;

    class Lock
    {
      public:
        enum class LockState
        {
            Unlocked,
            InputRequired,
            InputInvalid,
            Blocked,
            NewInputRequired,
            NewInputConfirmRequired,
            NewInputInvalid,
            ErrorOccurred
        };

        [[nodiscard]] LockState getState() const noexcept
        {
            return lockState;
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
        [[nodiscard]] bool isState(LockState state) const noexcept
        {
            return lockState == state;
        }
        [[nodiscard]] const std::string &getLockName() const noexcept
        {
            return lockName;
        }
        [[nodiscard]] const std::string &getNextUnlockAttemptFormattedTime() const noexcept
        {
            return nextUnlockAttemptFormattedTime;
        }
        void setNextUnlockAttemptFormattedTime(const std::string &time);

        void putNextChar(unsigned int c);
        /// removes a last character passed to Lock via putNextChar. The last character can not be popped
        void popChar();
        /// clear all characters passed to the Lock
        void clearAttempt() noexcept;
        /// consumes LockState::InputInvalid state and LockState::NewInputInvalid
        void consumeState() noexcept;

        explicit Lock(LockState state, unsigned int attemptsLeft = unlimitedNumOfAttempts)
            : lockState{state}, attemptsLeft{attemptsLeft}
        {}

      private:
        std::string lockName;
        LockState lockState       = LockState::Unlocked;
        unsigned int attemptsLeft = 0;
        std::string nextUnlockAttemptFormattedTime;

        std::vector<unsigned int> inputValue;
        unsigned int maxInputSize = defaultInputSize;
        unsigned int minInputSize = defaultInputSize;

        static constexpr unsigned int defaultInputSize       = 4;
        static constexpr unsigned int unlimitedNumOfAttempts = std::numeric_limits<unsigned int>::max();

        void setInputSizeBounds(unsigned int _minInputSize, unsigned int _maxInputSize)
        {
            minInputSize = _minInputSize;
            maxInputSize = _maxInputSize;
        }

        friend class PhoneLockHandler;
        friend class SimLockHandler;
    };

} // namespace locks

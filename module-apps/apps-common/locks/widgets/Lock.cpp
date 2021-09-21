// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include <log/log.hpp>

namespace locks
{
    void Lock::consumeState() noexcept
    {
        if (lockState == LockState::InputInvalid) {
            lockState = LockState::InputRequired;
        }
        else if (lockState == LockState::NewInputInvalid) {
            lockState = LockState::NewInputRequired;
        }
        else if (lockState == LockState::NewInputConfirmRequired) {
            lockState = LockState::NewInputRequired;
        }
    }

    void Lock::putNextChar(unsigned int c)
    {
        if (maxInputSize > inputValue.size()) {
            inputValue.push_back(c);
        }
    }

    void Lock::popChar()
    {
        if (!inputValue.empty()) {
            inputValue.pop_back();
        }
    }

    void Lock::clearAttempt() noexcept
    {
        inputValue.clear();
    }

    void Lock::setNextUnlockAttemptFormattedTime(const std::string &time)
    {
        nextUnlockAttemptFormattedTime = time;
    }

} // namespace locks

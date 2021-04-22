// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include <module-utils/log/log.hpp>

namespace lock
{
    void Lock::consumeState() noexcept
    {
        if (lockState == LockState::InputInvalidRetryRequired) {
            lockState = LockState::InputRequired;
        }
        else if (lockState == LockState::NewInputInvalid) {
            lockState = LockState::NewInputRequired;
        }
    }

    void Lock::putNextChar(unsigned int c)
    {
        if (maxInputSize > inputValue.size()) {
            inputValue.push_back(c);
        }
        if (canVerify() && autoActivate && onActivatedCallback != nullptr) {
            onActivatedCallback(lockType, inputValue);
        }
    }

    void Lock::popChar()
    {
        if (inputValue.size() > 0) {
            inputValue.pop_back();
        }
    }

    void Lock::clearAttempt() noexcept
    {
        inputValue.clear();
    }

    void Lock::activate()
    {
        if (!onActivatedCallback) {
            LOG_ERROR("Passcode verification callback null");
            return;
        }
        onActivatedCallback(lockType, inputValue);
        clearAttempt();
    }
} // namespace lock

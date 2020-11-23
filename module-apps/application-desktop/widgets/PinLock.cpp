// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PinLock.hpp"
#include "PinLockHandler.hpp"

namespace gui
{
    void PinLock::consumeState() noexcept
    {
        if (lockState == LockState::PasscodeInvalidRetryRequired) {
            lockState = LockState::PasscodeRequired;
        }
        else if (lockState == LockState::NewPasscodeInvalid) {
            lockState = LockState::NewPasscodeRequired;
        }
    }

    void PinLock::setNewPasscodeInvalidState() noexcept
    {
        if (lockState == LockState::NewPasscodeRequired) {
            lockState = LockState::NewPasscodeInvalid;
        }
    }

    void PinLock::putNextChar(unsigned int c)
    {
        if (maxPinSize > pinValue.size()) {
            pinValue.push_back(c);
        }
        if (canVerify() && autoActivate && onActivatedCallback != nullptr) {
            onActivatedCallback(pinValue);
        }
    }

    void PinLock::popChar()
    {
        if (pinValue.size() > 0) {
            pinValue.pop_back();
        }
    }

    void PinLock::clearAttempt() noexcept
    {
        pinValue.clear();
    }

    void PinLock::verify()
    {
        auto pinCopy = std::move(pinValue);
        clearAttempt();
        if (!onActivatedCallback) {
            LOG_ERROR("Passcode verification callback null");
            return;
        }
        onActivatedCallback(pinCopy);
    }
} // namespace gui

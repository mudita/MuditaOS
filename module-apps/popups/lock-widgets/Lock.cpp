// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include <module-utils/log/log.hpp>

namespace gui
{
    void Lock::consumeState() noexcept
    {
        if (lockState == LockState::Blocked) {
            lockState = LockState::InputRequired;
        }

        if (lockState == LockState::InvalidInputRetryRequired) {
            lockState = LockState::InputRequired;
        }
        //        else if (lockState == LockState::NewPasscodeInvalid) {
        //            lockState = LockState::NewPasscodeRequired;
        //        }
    }

    void Lock::setNewPasscodeInvalidState() noexcept
    {
        //        if (lockState == LockState::NewPasscodeRequired) {
        //            lockState = LockState::NewPasscodeInvalid;
        //        }
    }

    void Lock::putNextChar(unsigned int c)
    {
        if (maxPinSize > pinValue.size()) {
            pinValue.push_back(c);
        }
        if (canVerify() && autoActivate && onActivatedCallback != nullptr) {
            onActivatedCallback(pinValue);
        }
    }

    void Lock::popChar()
    {
        if (pinValue.size() > 0) {
            pinValue.pop_back();
        }
    }

    void Lock::clearAttempt() noexcept
    {
        pinValue.clear();
    }

    void Lock::activate()
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

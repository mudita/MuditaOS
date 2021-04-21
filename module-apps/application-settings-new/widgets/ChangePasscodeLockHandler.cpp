// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangePasscodeLockHandler.hpp"
#include "module-apps/locks/widgets/LockHash.hpp"

namespace gui
{
    ChangePasscodeLockHandler::ChangePasscodeLockHandler()
        : lock(Store::GSM::SIM::NONE, Lock::LockState::InputRequired, Lock::LockType::Screen)
    {}

    Lock::LockState ChangePasscodeLockHandler::checkPasscode(unsigned int currentLockPassHash)
    {
        return activateLock([this, currentLockPassHash](Lock::LockType, const std::vector<unsigned int> &pin) {
            const auto hash = GetHash(pin);
            if (hash == currentLockPassHash) {
                lock.lockState = Lock::LockState::NewInputRequired;
            }
            else {
                lock.lockState = Lock::LockState::InputInvalidRetryRequired;
            }
        });
    }

    Lock::LockState ChangePasscodeLockHandler::newPasscodeConfirmed()
    {
        return activateLock([this](Lock::LockType, const std::vector<unsigned int> &pin) {
            const auto newPasscodeConfirmedHash = GetHash(pin);
            if (newPasscodeHash == newPasscodeConfirmedHash) {
                lock.lockState = Lock::LockState::Unlocked;
            }
            else {
                lock.lockState = Lock::LockState::NewInputInvalid;
            }
        });
    }

    Lock::LockState ChangePasscodeLockHandler::newPasscodeProvided()
    {
        return activateLock([this](Lock::LockType, const std::vector<unsigned int> &pin) {
            if (pin.size() < lock.getMaxInputSize()) {
                lock.lockState = Lock::LockState::NewInputInvalidRetryRequired;
                return;
            }
            newPasscodeHash = GetHash(pin);
            lock.lockState  = Lock::LockState::NewInputConfirmRequired;
        });
    }

    Lock::LockState ChangePasscodeLockHandler::activateLock(OnActivatedCallback onActivatedCallback)
    {
        lock.onActivatedCallback = onActivatedCallback;
        lock.activate();
        return lock.lockState;
    }
} // namespace gui

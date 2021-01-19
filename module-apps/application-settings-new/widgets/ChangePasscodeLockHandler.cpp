// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangePasscodeLockHandler.hpp"
#include "application-desktop/widgets/PinHash.hpp"

namespace gui
{
    ChangePasscodeLockHandler::ChangePasscodeLockHandler()
        : lock(Store::GSM::SIM::NONE, PinLock::LockState::PasscodeRequired, PinLock::LockType::Screen)
    {}

    PinLock::LockState ChangePasscodeLockHandler::checkPasscode(unsigned int currentLockPassHash)
    {
        return activateLock([this, currentLockPassHash](PinLock::LockType, const std::vector<unsigned int> &pin) {
            const auto hash = GetPinHash(pin);
            if (hash == currentLockPassHash) {
                lock.lockState = PinLock::LockState::NewPasscodeRequired;
            }
            else {
                lock.lockState = PinLock::LockState::PasscodeInvalidRetryRequired;
            }
        });
    }

    PinLock::LockState ChangePasscodeLockHandler::newPasscodeConfirmed()
    {
        return activateLock([this](PinLock::LockType, const std::vector<unsigned int> &pin) {
            const auto newPasscodeConfirmedHash = GetPinHash(pin);
            if (newPasscodeHash == newPasscodeConfirmedHash) {
                lock.lockState = PinLock::LockState::Unlocked;
            }
            else {
                lock.lockState = PinLock::LockState::NewPasscodeInvalid;
            }
        });
    }

    PinLock::LockState ChangePasscodeLockHandler::newPasscodeProvided()
    {
        return activateLock([this](PinLock::LockType, const std::vector<unsigned int> &pin) {
            if (pin.size() < lock.getMaxPinSize()) {
                lock.lockState = PinLock::LockState::NewPasscodeInvalidRetryRequired;
                return;
            }
            newPasscodeHash = GetPinHash(pin);
            lock.lockState  = PinLock::LockState::NewPasscodeConfirmRequired;
        });
    }

    PinLock::LockState ChangePasscodeLockHandler::activateLock(OnActivatedCallback onActivatedCallback)
    {
        lock.onActivatedCallback = onActivatedCallback;
        lock.activate();
        return lock.lockState;
    }
} // namespace gui

//// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/Utils.hpp>
#include "PhoneLockHandler.hpp"
#include "locks/widgets/LockHash.hpp"

namespace lock
{
    constexpr unsigned int default_attempts = 4;

    PhoneLockHandler::PhoneLockHandler() : lock(Lock::LockState::InputRequired, default_attempts)
    {}

    void PhoneLockHandler::enablePhoneLock(bool _phoneLockEnabled)
    {
        phoneLockEnabled = _phoneLockEnabled;
    }

    void PhoneLockHandler::setPhoneLockHash(const std::string &value)
    {
        if (!value.empty()) {
            phoneLockHash = utils::getNumericValue<unsigned int>(value);
        }
        else {
            phoneLockHash = 0;
        }
    }

    void PhoneLockHandler::verifyCallbacks()
    {
        if (!onPhoneLockCallback || !onPhoneUnlockCallback || !onPhoneInputRequiredCallback) {
            throw std::runtime_error("Missing PhoneLockHandlers!");
        }
    }

    void PhoneLockHandler::addOnPhoneLockCallback(PhoneLockHandler::OnPhoneLockCallback &&onLock) noexcept
    {
        onPhoneLockCallback = std::move(onLock);
    }

    void PhoneLockHandler::addOnPhoneUnlockCallback(PhoneLockHandler::OnPhoneUnlockCallback &&onUnlock) noexcept
    {
        onPhoneUnlockCallback = std::move(onUnlock);
    }

    void PhoneLockHandler::addOnPhoneInputRequiredCallback(
        PhoneLockHandler::OnPhoneInputRequiredCallback &&onInputRequired) noexcept
    {
        onPhoneInputRequiredCallback = std::move(onInputRequired);
    }

    sys::MessagePointer PhoneLockHandler::handleUnlockRequest()
    {
        verifyCallbacks();

        // Check if phoneLock has not been disabled by setting
        if (!phoneLockEnabled) {
            onPhoneUnlockCallback();
            return sys::msgHandled();
        }

        if (lock.isState(Lock::LockState::Unlocked)) {
            onPhoneUnlockCallback();
        }
        else if (lock.isState(Lock::LockState::Blocked)) {
            onPhoneInputRequiredCallback(lock);
        }
        else if (!lock.isState(Lock::LockState::Unlocked)) {

            lock.lockState = Lock::LockState::InputRequired;
            onPhoneInputRequiredCallback(lock);
        }

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleLockRequest()
    {
        verifyCallbacks();

        lock.lockState = Lock::LockState::InputRequired;
        onPhoneLockCallback();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::verifyPhoneLockInput(const std::vector<unsigned int> &inputData)
    {
        verifyCallbacks();

        const uint32_t hash = getHash(inputData);
        lock.attemptsLeft--;

        if (phoneLockHash == hash) {
            lock.lockState    = Lock::LockState::Unlocked;
            lock.attemptsLeft = default_attempts;
            onPhoneUnlockCallback();
            return sys::msgHandled();
        }
        else if (lock.attemptsLeft > 0) {
            lock.lockState = Lock::LockState::InputInvalidRetryRequired;
        }
        else {
            lock.lockState = Lock::LockState::Blocked;
        }

        onPhoneInputRequiredCallback(lock);

        return sys::msgHandled();
    }

    bool PhoneLockHandler::isPhoneLocked() const noexcept
    {
        return !lock.isState(Lock::LockState::Unlocked);
    }

} // namespace lock

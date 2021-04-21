//// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/Utils.hpp>
#include "PhoneLockObserver.hpp"
#include "popups/lock-widgets/LockHash.hpp"

namespace lock
{
    constexpr unsigned int default_attempts = 4;

    PhoneLockObserver::PhoneLockObserver()
    {
        lock = new Lock(Lock::LockState::InputRequired, default_attempts);

        lock->onActivatedCallback = [this](Lock::LockType type, const std::vector<unsigned int> &data) {
            const uint32_t hash = GetHash(data);
            lock->attemptsLeft--;

            if (phoneLockHash == hash) {
                lock->lockState    = Lock::LockState::Unlocked;
                lock->attemptsLeft = default_attempts;
                onPhoneUnlockCallback();
                return;
            }
            else if (lock->attemptsLeft > 0) {
                lock->lockState = Lock::LockState::InputInvalidRetryRequired;
            }
            else {
                lock->lockState = Lock::LockState::Blocked;
            }

            onPhonePasscodeRequiredCallback(lock);
        };
    }

    PhoneLockObserver::~PhoneLockObserver()
    {
        delete lock;
    }

    void PhoneLockObserver::enablePhoneLock(bool _phoneLockEnabled)
    {
        phoneLockEnabled = _phoneLockEnabled;
    }

    void PhoneLockObserver::setPhoneLockHash(const std::string &value)
    {
        if (!value.empty()) {
            phoneLockHash = utils::getNumericValue<unsigned int>(value);
        }
        else {
            phoneLockHash = 0;
        }
    }

    void PhoneLockObserver::connect(sys::Service *owner)
    {
        owner->connect(typeid(LockPhone),
                       [this](sys::Message *request) -> sys::MessagePointer { return handleLockRequest(); });
        owner->connect(typeid(UnlockPhone),
                       [this](sys::Message *request) -> sys::MessagePointer { return handleUnlockRequest(); });
    }

    void PhoneLockObserver::subscribe(OnPhoneLockCallback &&onLock,
                                      OnPhoneUnlockCallback &&onUnlock,
                                      OnPhonePasscodeRequiredCallback &&onPasscodeRequired) noexcept
    {
        onPhoneLockCallback             = std::move(onLock);
        onPhoneUnlockCallback           = std::move(onUnlock);
        onPhonePasscodeRequiredCallback = std::move(onPasscodeRequired);
    }

    sys::MessagePointer PhoneLockObserver::handleUnlockRequest()
    {
        // Check if phoneLock has not been disabled by setting
        if (!phoneLockEnabled) {
            onPhoneUnlockCallback();
        }
        else {
            if (lock->isState(Lock::LockState::Unlocked)) {
                onPhoneUnlockCallback();
            }
            else if (lock->isState(Lock::LockState::Blocked)) {
                onPhonePasscodeRequiredCallback(lock);
            }
            else if (!lock->isState(Lock::LockState::Unlocked)) {

                lock->lockState = Lock::LockState::InputRequired;
                onPhonePasscodeRequiredCallback(lock);
            }
        }

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockObserver::handleLockRequest()
    {
        lock->lockState = Lock::LockState::InputRequired;
        onPhoneLockCallback();

        return sys::msgHandled();
    }

    bool PhoneLockObserver::isPhoneLocked()
    {
        return !lock->isState(Lock::LockState::Unlocked);
    }

} // namespace lock

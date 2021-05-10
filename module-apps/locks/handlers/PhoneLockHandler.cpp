//// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockHandler.hpp"

#include <service-appmgr/service-appmgr/Controller.hpp>
#include <locks/widgets/LockHash.hpp>
#include <Utils.hpp>
#include <memory>

#include <module-apps/popups/data/PopupRequestParams.hpp>

namespace locks
{
    constexpr unsigned int default_attempts = 4;

    PhoneLockHandler::PhoneLockHandler(sys::Service *owner)
        : owner(owner), lock(Lock::LockState::InputRequired, default_attempts)
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

    void PhoneLockHandler::PhoneLockAction()
    {
        app::manager::Controller::sendAction(owner,
                                             app::manager::actions::ShowPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::PhoneLock));
        owner->bus.sendMulticast(std::make_shared<locks::LockedPhone>(), sys::BusChannel::PhoneLockChanges);
    }

    void PhoneLockHandler::PhoneUnlockPopupsCloseAction()
    {
        app::manager::Controller::sendAction(owner,
                                             app::manager::actions::AbortPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::InputLock));
        if (!isPhoneLocked()) {
            app::manager::Controller::sendAction(owner,
                                                 app::manager::actions::AbortPopup,
                                                 std::make_unique<gui::PopupRequestParams>(gui::popup::ID::PhoneLock));
        }
    }

    void PhoneLockHandler::PhoneUnlockAction()
    {
        PhoneUnlockPopupsCloseAction();
        owner->bus.sendMulticast(std::make_shared<locks::UnlockedPhone>(), sys::BusChannel::PhoneLockChanges);
    }

    void PhoneLockHandler::PhoneInputRequiredAction()
    {
        app::manager::Controller::sendAction(
            owner,
            app::manager::actions::ShowPopup,
            std::make_unique<gui::PhoneUnlockInputRequestParams>(gui::popup::ID::InputLock, lock));
    }

    sys::MessagePointer PhoneLockHandler::handleUnlockRequest()
    {
        // Check if phoneLock has not been disabled by setting
        if (!phoneLockEnabled) {
            PhoneUnlockAction();
            return sys::msgHandled();
        }

        if (lock.isState(Lock::LockState::Unlocked)) {
            PhoneUnlockAction();
        }
        else if (lock.isState(Lock::LockState::Blocked)) {
            PhoneInputRequiredAction();
        }
        else if (!lock.isState(Lock::LockState::Unlocked)) {

            lock.lockState = Lock::LockState::InputRequired;
            PhoneInputRequiredAction();
        }

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleLockRequest()
    {
        lock.lockState = Lock::LockState::InputRequired;
        PhoneLockAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::verifyPhoneLockInput(const std::vector<unsigned int> &inputData)
    {
        const uint32_t hash = getHash(inputData);
        lock.attemptsLeft--;

        if (phoneLockHash == hash) {
            lock.lockState    = Lock::LockState::Unlocked;
            lock.attemptsLeft = default_attempts;
            PhoneUnlockAction();
            return sys::msgHandled();
        }
        else if (lock.attemptsLeft > 0) {
            lock.lockState = Lock::LockState::InputInvalidRetryRequired;
        }
        else {
            lock.lockState = Lock::LockState::Blocked;
        }

        PhoneInputRequiredAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleUnlockCancelRequest()
    {
        PhoneUnlockPopupsCloseAction();
        return sys::msgHandled();
    }

    bool PhoneLockHandler::isPhoneLocked() const noexcept
    {
        return !lock.isState(Lock::LockState::Unlocked);
    }
} // namespace locks

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockHandler.hpp"

#include <service-appmgr/Controller.hpp>
#include <locks/widgets/LockHash.hpp>
#include <Utils.hpp>
#include <memory>

#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace locks
{
    PhoneLockHandler::PhoneLockHandler(sys::Service *owner, std::shared_ptr<settings::Settings> settings)
        : owner(owner), lock(Lock::LockState::InputRequired, initialNoCooldownAttemptsLeft),
          settings(std::move(settings))
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

    void PhoneLockHandler::setPhoneLockTime(const time_t time)
    {
        lockedTill = time;
    }

    void PhoneLockHandler::setNextUnlockAttemptCooldownTime(const time_t time)
    {
        nextUnlockAttemptCooldownTime = time;
    }

    void PhoneLockHandler::setNoCooldownAttemptsLeft(const unsigned int attemptsNumber)
    {
        lock.attemptsLeft = attemptsNumber;
    }

    void PhoneLockHandler::setPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction)
    {
        if (phoneLockInputTypeAction != _phoneLockInputTypeAction) {
            phoneLockInputTypeAction = _phoneLockInputTypeAction;
            lock.attemptsLeft        = initialNoCooldownAttemptsLeft;
            storedInputData.clear();
        }
    }

    bool PhoneLockHandler::checkPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction)
    {
        return phoneLockInputTypeAction == _phoneLockInputTypeAction;
    }

    void PhoneLockHandler::phoneLockAction()
    {
        phoneState = PhoneState::Locked;
        app::manager::Controller::sendAction(owner,
                                             app::manager::actions::ShowPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::PhoneLock));
        owner->bus.sendMulticast(std::make_shared<locks::LockedPhone>(), sys::BusChannel::PhoneLockChanges);
    }

    void PhoneLockHandler::phoneUnlockPopupsCloseAction()
    {
        if (!isPhoneLocked()) {
            app::manager::Controller::sendAction(owner,
                                                 app::manager::actions::AbortPopup,
                                                 std::make_unique<gui::PopupRequestParams>(gui::popup::ID::PhoneLock));
            app::manager::Controller::sendAction(
                owner,
                app::manager::actions::AbortPopup,
                std::make_unique<gui::PopupRequestParams>(gui::popup::ID::PhoneLockInfo));
        }

        app::manager::Controller::sendAction(owner,
                                             app::manager::actions::AbortPopup,
                                             std::make_unique<gui::PopupRequestParams>(gui::popup::ID::PhoneLockInput));
    }

    void PhoneLockHandler::phoneUnlockAction()
    {
        phoneState = PhoneState::Unlocked;
        phoneUnlockPopupsCloseAction();
        owner->bus.sendMulticast(std::make_shared<locks::UnlockedPhone>(), sys::BusChannel::PhoneLockChanges);
    }

    void PhoneLockHandler::phoneInputRequiredAction()
    {
        app::manager::Controller::sendAction(owner,
                                             app::manager::actions::ShowPopup,
                                             std::make_unique<gui::PhoneUnlockInputRequestParams>(
                                                 gui::popup::ID::PhoneLockInput, lock, phoneLockInputTypeAction));
    }

    void PhoneLockHandler::phoneLockEnableAction()
    {
        setPhoneLockAvailabilityInSettings(true);
        phoneLockChangeAction();
    }

    void PhoneLockHandler::phoneLockDisableAction()
    {
        setPhoneLockAvailabilityInSettings(false);
        phoneLockChangeInfoAction();
        phoneUnlockPopupsCloseAction();
    }

    void PhoneLockHandler::phoneLockChangeAction()
    {
        setPhoneLockInSettings();
        phoneLockChangeInfoAction();
        phoneUnlockPopupsCloseAction();
    }

    void PhoneLockHandler::phoneLockSetAction()
    {
        setPhoneLockAvailabilityInSettings(true);
        setPhoneLockInSettings();

        lock.lockState = Lock::LockState::NewInputRequired;
        owner->bus.sendMulticast(std::make_shared<locks::SetConfirmedPhoneLock>(), sys::BusChannel::PhoneLockChanges);
    }

    void PhoneLockHandler::phoneLockChangeInfoAction()
    {
        app::manager::Controller::sendAction(owner,
                                             app::manager::actions::ShowPopup,
                                             std::make_unique<gui::PhoneUnlockInputRequestParams>(
                                                 gui::popup::ID::PhoneLockChangeInfo, lock, phoneLockInputTypeAction));
    }

    void PhoneLockHandler::phoneExternalUnlockInfoAction()
    {
        owner->bus.sendMulticast(std::make_shared<locks::ExternalUnLockPhoneInfo>(lock.getAttemptsLeft()),
                                 sys::BusChannel::PhoneLockChanges);
    }

    void PhoneLockHandler::setPhoneLockInSettings()
    {
        phoneLockHash = getHash(storedInputData);
        settings->setValue(::settings::SystemProperties::lockPassHash,
                           std::to_string(phoneLockHash),
                           ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::setPhoneLockAvailabilityInSettings(bool value)
    {
        phoneLockEnabled = value;
        settings->setValue(settings::SystemProperties::lockScreenPasscodeIsOn,
                           std::to_string(phoneLockEnabled),
                           settings::SettingsScope::Global);
    }

    void PhoneLockHandler::savePhoneLockTime()
    {
        settings->setValue(::settings::SystemProperties::unlockBlockTime,
                           std::to_string(lockedTill),
                           ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::saveNextUnlockAttemptCooldownTime()
    {
        settings->setValue(::settings::SystemProperties::unlockAttemptCooldownTime,
                           std::to_string(nextUnlockAttemptCooldownTime),
                           ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::saveNoCooldownAttemptsLeft()
    {
        settings->setValue(::settings::SystemProperties::noCooldownAttemptsLeft,
                           std::to_string(lock.attemptsLeft),
                           ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::storePhoneLockInformation()
    {
        savePhoneLockTime();
        saveNextUnlockAttemptCooldownTime();
        saveNoCooldownAttemptsLeft();
    }

    sys::MessagePointer PhoneLockHandler::handleUnlockRequest()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::Unlock);

        // Check if phoneLock has not been disabled by setting
        if (!phoneLockEnabled) {
            phoneUnlockAction();
            return sys::msgHandled();
        }
        if (lock.isState(Lock::LockState::Blocked)) {
            phoneInputRequiredAction();
        }
        else if (!lock.isState(Lock::LockState::Unlocked)) {
            lock.lockState = Lock::LockState::InputRequired;
            phoneInputRequiredAction();
        }

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleLockRequest()
    {
        lock.lockState = Lock::LockState::InputRequired;
        phoneLockAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleEnablePhoneLock()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::Enable);

        lock.lockState    = Lock::LockState::NewInputRequired;
        lock.attemptsLeft = Lock::unlimitedNumOfAttempts;

        phoneInputRequiredAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleDisablePhoneLock()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::Disable);

        lock.lockState    = Lock::LockState::InputRequired;
        lock.attemptsLeft = Lock::unlimitedNumOfAttempts;

        phoneInputRequiredAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleChangePhoneLock()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::ConfirmCurrent);

        lock.lockState    = Lock::LockState::InputRequired;
        lock.attemptsLeft = Lock::unlimitedNumOfAttempts;

        phoneInputRequiredAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleSetPhoneLock()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::Set);

        lock.lockState    = Lock::LockState::NewInputRequired;
        lock.attemptsLeft = Lock::unlimitedNumOfAttempts;

        phoneInputRequiredAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleSkipSetPhoneLock()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::Set);

        lock.lockState    = Lock::LockState::NewInputRequired;
        lock.attemptsLeft = Lock::unlimitedNumOfAttempts;

        owner->bus.sendMulticast(std::make_shared<locks::SkippedSetPhoneLock>(), sys::BusChannel::PhoneLockChanges);
        return sys::msgHandled();
    }

    void PhoneLockHandler::checkNewPhoneLock()
    {
        setPhoneLockInputTypeAction(PhoneLockInputTypeAction::Change);

        lock.lockState    = Lock::LockState::NewInputRequired;
        lock.attemptsLeft = Lock::unlimitedNumOfAttempts;

        phoneInputRequiredAction();
    }

    sys::MessagePointer PhoneLockHandler::handlePhoneLockInput(LockInput inputData)
    {
        if (checkPhoneLockInputTypeAction(PhoneLockInputTypeAction::Enable) ||
            checkPhoneLockInputTypeAction(PhoneLockInputTypeAction::Change) ||
            checkPhoneLockInputTypeAction(PhoneLockInputTypeAction::Set)) {
            return verifyPhoneLockChangeInput(inputData);
        }
        else {
            return verifyPhoneUnlockInput(inputData);
        }
    }

    void PhoneLockHandler::comparePhoneLockHashCode(LockInput inputData)
    {
        const uint32_t hash = getHash(inputData);
        lock.attemptsLeft--;

        if (phoneLockHash == hash) {
            lock.lockState    = Lock::LockState::Unlocked;
            lock.attemptsLeft = initialNoCooldownAttemptsLeft;
            resetLockTime();
        }
        else if (lock.attemptsLeft > 0) {
            lock.lockState = Lock::LockState::InputInvalid;
            resetLockTime();
        }
        else {
            increaseLockTime();
            lock.lockState = Lock::LockState::Blocked;
        }
        storePhoneLockInformation();
        lock.setNextUnlockAttemptCooldownTimeMinutes(nextUnlockAttemptCooldownTime / 60);
    }

    sys::MessagePointer PhoneLockHandler::verifyPhoneUnlockInput(LockInput inputData)
    {
        comparePhoneLockHashCode(inputData);

        if (lock.isState(Lock::LockState::Unlocked)) {
            resolvePhoneLockAction();
        }
        else {
            phoneInputRequiredAction();
        }

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::verifyPhoneLockChangeInput(LockInput inputData)
    {
        if (lock.isState(Lock::LockState::NewInputRequired) || lock.isState(Lock::LockState::NewInputInvalid)) {
            storedInputData = inputData;
            lock.lockState  = Lock::LockState::NewInputConfirmRequired;
        }
        else if (lock.isState(Lock::LockState::NewInputConfirmRequired)) {
            if (storedInputData == inputData) {
                resolvePhoneLockAction();
                return sys::msgHandled();
            }
            else {
                lock.lockState = Lock::LockState::NewInputInvalid;
            }
        }

        phoneInputRequiredAction();

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleExternalUnlockRequest(LockInput inputData)
    {
        if (!phoneLockEnabled) {
            phoneUnlockAction();
            return sys::msgHandled();
        }

        if (lock.isState(Lock::LockState::Blocked)) {
            phoneExternalUnlockInfoAction();
            return sys::msgHandled();
        }

        comparePhoneLockHashCode(inputData);

        if (lock.isState(Lock::LockState::Unlocked)) {
            phoneUnlockAction();
        }
        else {
            phoneExternalUnlockInfoAction();
        }

        return sys::msgHandled();
    }

    sys::MessagePointer PhoneLockHandler::handleExternalAvailabilityChange(bool value)
    {
        setPhoneLockAvailabilityInSettings(value);
        return sys::msgHandled();
    }

    void PhoneLockHandler::resolvePhoneLockAction()
    {
        switch (phoneLockInputTypeAction) {
        case PhoneLockInputTypeAction::Unlock:
            return phoneUnlockAction();
        case PhoneLockInputTypeAction::Enable:
            return phoneLockEnableAction();
        case PhoneLockInputTypeAction::Disable:
            return phoneLockDisableAction();
        case PhoneLockInputTypeAction::ConfirmCurrent:
            return checkNewPhoneLock();
        case PhoneLockInputTypeAction::Change:
            return phoneLockChangeAction();
        case PhoneLockInputTypeAction::Set:
            return phoneLockSetAction();
        default:
            return;
        }
    }

    sys::MessagePointer PhoneLockHandler::handleUnlockCancelRequest()
    {
        phoneUnlockPopupsCloseAction();
        return sys::msgHandled();
    }

    bool PhoneLockHandler::isPhoneLocked() const noexcept
    {
        return phoneState == PhoneState::Locked;
    }

    void PhoneLockHandler::increaseLockTime() noexcept
    {
        if (lockedTill != 0) {
            nextUnlockAttemptCooldownTime *= 2;
        }
        lockedTill = std::time(nullptr) + nextUnlockAttemptCooldownTime;
    }

    void PhoneLockHandler::resetLockTime() noexcept
    {
        lockedTill                    = 0;
        nextUnlockAttemptCooldownTime = initialCooldownTime;
    }

} // namespace locks

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockHandler.hpp"

#include <locks/widgets/LockHash.hpp>

#include <service-appmgr/Controller.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <Timers/TimerFactory.hpp>
#include <time/time_conversion_factory.hpp>

namespace locks
{
    PhoneLockHandler::PhoneLockHandler(sys::Service *owner, std::shared_ptr<settings::Settings> settings)
        : owner(owner), settings(std::move(settings)),
          lock(Lock::LockState::InputRequired, initialNoLockTimeAttemptsLeft)
    {
        phoneLockTimer = sys::TimerFactory::createPeriodicTimer(
            owner, phoneLockTimerName, std::chrono::seconds{(1)}, [this](sys::Timer &) {
                if (lockedFor == 0) {
                    phoneLockTimer.stop();
                    phoneLockTimeUpdateInfoAction(LockTimerState::Stop);
                    return;
                }

                if (lockedFor == nextUnlockAttemptLockTime) {
                    phoneLockTimeUpdateInfoAction(LockTimerState::Start);
                }
                else if (lockedFor % utils::secondsInMinute == 0 || lockedFor <= utils::secondsInMinute) {
                    phoneLockTimeUpdateInfoAction(LockTimerState::Counting);
                }

                --lockedFor;

                if (lockedFor == 0) {
                    savePhoneLockTime();
                }

                return;
            });
    }

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
        if (time != 0) {
            lockedFor      = time;
            lock.lockState = Lock::LockState::Blocked;
            phoneLockTimeUpdateInfoAction(LockTimerState::Start);
            phoneLockTimer.start();
        }
    }

    void PhoneLockHandler::setNextUnlockAttemptLockTime(time_t time)
    {
        nextUnlockAttemptLockTime = time;
    }

    void PhoneLockHandler::setNoLockTimeAttemptsLeft(const unsigned int attemptsNumber)
    {
        lock.attemptsLeft = attemptsNumber;
    }

    void PhoneLockHandler::setPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction)
    {
        if (phoneLockInputTypeAction != _phoneLockInputTypeAction) {
            phoneLockInputTypeAction = _phoneLockInputTypeAction;
            lock.attemptsLeft        = initialNoLockTimeAttemptsLeft;
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

    void PhoneLockHandler::phoneLockTimeUpdateInfoAction(LockTimerState state)
    {
        std::string textToPrint;
        auto rebuildRequired = state == LockTimerState::Start;

        if (state == LockTimerState::Start || state == LockTimerState::Counting) {
            if (lockedFor <= utils::secondsInMinute) {
                using utils::time::Duration;
                const Duration remainingDuration{static_cast<uint32_t>(std::time_t{lockedFor})};
                textToPrint = remainingDuration.str(Duration::DisplayedFormat::Fixed0M0S);

                if (lockedFor == utils::secondsInMinute) {
                    rebuildRequired = true;
                }
            }
            else {
                textToPrint = std::to_string(lockedFor / utils::secondsInMinute) +
                              utils::translate("phone_lock_blocked_information_minutes");
            }
        }

        owner->bus.sendMulticast(std::make_shared<locks::PhoneLockTimeUpdate>(textToPrint, rebuildRequired),
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
        settings->setValue(
            ::settings::SystemProperties::unlockLockTime, std::to_string(lockedFor), ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::saveNextUnlockAttemptLockTime()
    {
        settings->setValue(::settings::SystemProperties::unlockAttemptLockTime,
                           std::to_string(nextUnlockAttemptLockTime),
                           ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::saveNoLockTimeAttemptsLeft()
    {
        settings->setValue(::settings::SystemProperties::noLockTimeAttemptsLeft,
                           std::to_string(lock.attemptsLeft),
                           ::settings::SettingsScope::Global);
    }

    void PhoneLockHandler::setPhoneLockTimeInformationInSettings()
    {
        savePhoneLockTime();
        saveNextUnlockAttemptLockTime();
        saveNoLockTimeAttemptsLeft();
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
            if (lockedFor == 0) {
                lock.lockState = Lock::LockState::InputRequired;
            }
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
        lock.lockState    = Lock::LockState::InputRequired;
        lock.attemptsLeft = initialNoLockTimeAttemptsLeft;
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
        lock.attemptsLeft   = (lock.attemptsLeft > 0 ? lock.attemptsLeft - 1 : 0);

        if (phoneLockHash == hash) {
            lock.lockState    = Lock::LockState::Unlocked;
            lock.attemptsLeft = initialNoLockTimeAttemptsLeft;
            resetLockTime();
        }
        else if (lock.attemptsLeft > 0) {
            lock.lockState = Lock::LockState::InputInvalid;
        }
        else {
            increaseLockTime();
            lock.lockState = Lock::LockState::Blocked;
        }

        broadcastLockTime();
        setPhoneLockTimeInformationInSettings();
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
            broadcastLockTime();
            if (lockedFor != 0) {
                return sys::msgHandled();
            }
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
        nextUnlockAttemptLockTime != 0 ? nextUnlockAttemptLockTime *= locks::phoneLockTimeMultiplier
                                       : nextUnlockAttemptLockTime = initialLockTime;
        lockedFor = nextUnlockAttemptLockTime;

        phoneLockTimer.start();
        phoneLockTimeUpdateInfoAction(LockTimerState::Start);

        if (lockedFor < utils::secondsInMinute) {
            lock.setNextUnlockAttemptFormattedTime(std::to_string(lockedFor) +
                                                   utils::translate("phone_lock_blocked_information_seconds"));
        }
        else if (lockedFor >= utils::secondsInMinute && lockedFor < 2 * utils::secondsInMinute) {
            lock.setNextUnlockAttemptFormattedTime(utils::translate("phone_lock_blocked_information_minute"));
        }
        else {
            lock.setNextUnlockAttemptFormattedTime(std::to_string(lockedFor / utils::secondsInMinute) +
                                                   utils::translate("phone_lock_blocked_information_minutes"));
        }
    }

    void PhoneLockHandler::resetLockTime() noexcept
    {
        lockedFor                 = 0;
        nextUnlockAttemptLockTime = 0;
        phoneLockTimer.stop();
    }

    void PhoneLockHandler::broadcastLockTime() noexcept
    {
        owner->bus.sendMulticast(
            std::make_shared<locks::NextPhoneUnlockAttemptLockTime>(std::time(nullptr) + lockedFor),
            sys::BusChannel::PhoneLockChanges);
    }

} // namespace locks

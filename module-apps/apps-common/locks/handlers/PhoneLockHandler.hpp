// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/widgets/Lock.hpp>
#include <locks/data/PhoneLockMessages.hpp>
#include <locks/data/LockData.hpp>

#include <service-db/Settings.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

namespace locks
{
    constexpr auto phoneLockTimerName                    = "PhoneLockTimer";
    constexpr unsigned int initialNoLockTimeAttemptsLeft = 3;
    constexpr time_t initialLockTime                     = 15;
    constexpr unsigned int phoneLockTimeMultiplier       = 2;

    class PhoneLockHandler
    {
      private:
        enum class PhoneState
        {
            Locked,
            Unlocked,
        };

        enum class LockTimerState
        {
            Start,
            Counting,
            Stop,
        };

        sys::Service *owner;
        std::shared_ptr<settings::Settings> settings;
        Lock lock;
        bool phoneLockEnabled = true;

        PhoneState phoneState                             = PhoneState::Locked;
        PhoneLockInputTypeAction phoneLockInputTypeAction = PhoneLockInputTypeAction::Unlock;

        unsigned int phoneLockHash = std::numeric_limits<unsigned>::max();
        std::vector<unsigned int> storedInputData;

        sys::TimerHandle phoneLockTimer;
        time_t lockedFor                 = 0;
        time_t nextUnlockAttemptLockTime = 0;

        void setPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction);
        bool checkPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction);

        void setPhoneLockInSettings();
        void setPhoneLockAvailabilityInSettings(bool value);
        void setPhoneLockTimeInformationInSettings();
        void savePhoneLockTime();
        void saveNextUnlockAttemptLockTime();
        void saveNoLockTimeAttemptsLeft();

        void phoneLockAction();
        void phoneUnlockAction();
        void phoneLockEnableAction();
        void phoneLockDisableAction();
        void phoneLockChangeAction();
        void phoneLockSetAction();
        void phoneInputRequiredAction();
        void phoneUnlockPopupsCloseAction();
        void phoneLockChangeInfoAction();
        void phoneExternalUnlockInfoAction();
        void phoneLockTimeUpdateInfoAction(LockTimerState state);

        void checkNewPhoneLock();
        void resolvePhoneLockAction();

        void comparePhoneLockHashCode(LockInput inputData);
        sys::MessagePointer verifyPhoneUnlockInput(LockInput inputData);
        sys::MessagePointer verifyPhoneLockChangeInput(LockInput inputData);

      public:
        explicit PhoneLockHandler(sys::Service *owner, std::shared_ptr<settings::Settings> settings);

        sys::MessagePointer handleLockRequest();
        sys::MessagePointer handleUnlockRequest();
        sys::MessagePointer handleUnlockCancelRequest();
        sys::MessagePointer handleEnablePhoneLock();
        sys::MessagePointer handleDisablePhoneLock();
        sys::MessagePointer handleChangePhoneLock();
        sys::MessagePointer handleSetPhoneLock();
        sys::MessagePointer handleSkipSetPhoneLock();
        sys::MessagePointer handlePhoneLockInput(LockInput inputData);
        sys::MessagePointer handleExternalUnlockRequest(LockInput inputData);
        sys::MessagePointer handleExternalAvailabilityChange(bool value);

        void enablePhoneLock(bool _phoneLockEnabled);
        void setPhoneLockHash(const std::string &value);
        [[nodiscard]] bool isPhoneLocked() const noexcept;

        void setPhoneLockTime(time_t time);
        void setNextUnlockAttemptLockTime(time_t time);
        void setNoLockTimeAttemptsLeft(unsigned int attemptsNumber);
        void increaseLockTime() noexcept;
        void setNextUnlockAttemptFormattedTime() noexcept;
        void resetLockTime() noexcept;
        void broadcastLockTime() noexcept;
    };

} // namespace locks

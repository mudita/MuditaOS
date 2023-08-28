// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
    constexpr unsigned initialNoLockTimeAttemptsLeft     = 3;
    constexpr std::time_t initialLockTime                = 15;
    constexpr unsigned phoneLockTimeMultiplier           = 2;

    class PhoneLockHandler
    {
      public:
        enum class RequestReason
        {
            Default = 0, // For default behaviors
            MTPUnlock
        };

      private:
        enum class PhoneState
        {
            Locked,
            Unlocked
        };

        enum class LockTimerState
        {
            Start,
            Counting,
            Stop
        };

        sys::Service *owner = nullptr;
        std::shared_ptr<settings::Settings> settings;
        Lock lock;
        bool phoneLockEnabled = true;

        PhoneState phoneState                             = PhoneState::Locked;
        PhoneLockInputTypeAction phoneLockInputTypeAction = PhoneLockInputTypeAction::Unlock;

        unsigned phoneLockHash = std::numeric_limits<unsigned>::max();
        std::vector<unsigned> storedInputData;

        sys::TimerHandle phoneLockTimer;
        std::time_t lockedFor                 = 0;
        std::time_t nextUnlockAttemptLockTime = 0;

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
        void phoneInputRequiredAction(RequestReason reqReason = RequestReason::Default);
        void phoneUnlockPopupsCloseAction();
        void phoneBlockedPopupsCloseAction();
        void phoneLockChangeInfoAction();
        void phoneLockTimeUpdateInfoAction(LockTimerState state);

        void checkNewPhoneLock();
        void resolvePhoneLockAction();

        void processPhoneLockHashCode(LockInput inputData);
        sys::MessagePointer verifyPhoneUnlockInput(LockInput inputData);
        sys::MessagePointer verifyPhoneLockChangeInput(LockInput inputData);

      public:
        PhoneLockHandler(sys::Service *owner, std::shared_ptr<settings::Settings> settings);

        sys::MessagePointer handleLockRequest();
        sys::MessagePointer handleUnlockRequest(RequestReason reqReason = RequestReason::Default);
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

        void setPhoneLockTime(std::time_t time);
        void setNextUnlockAttemptLockTime(std::time_t time);
        void setNoLockTimeAttemptsLeft(unsigned attemptsNumber);
        void increaseLockTime() noexcept;
        void setNextUnlockAttemptFormattedTime() noexcept;
        void resetLockTime() noexcept;
        void broadcastLockTime() noexcept;
    };
} // namespace locks

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/widgets/Lock.hpp>
#include <locks/data/PhoneLockMessages.hpp>
#include <locks/data/LockData.hpp>

#include <service-db/service-db/Settings.hpp>
#include <module-sys/Service/Service.hpp>

namespace locks
{
    class PhoneLockHandler
    {
      private:
        enum class PhoneState
        {
            Locked,
            Unlocked,
        };

        sys::Service *owner;
        Lock lock;
        bool phoneLockEnabled                             = true;
        unsigned int phoneLockHash                        = std::numeric_limits<unsigned>::max();
        PhoneState phoneState                             = PhoneState::Locked;
        PhoneLockInputTypeAction phoneLockInputTypeAction = PhoneLockInputTypeAction::Unlock;
        std::shared_ptr<settings::Settings> settings;
        std::vector<unsigned int> storedInputData;
        static constexpr unsigned int initialNoCooldownAttemptsLeft = 3;
        time_t lockedTill                                           = 0;
        static constexpr time_t initialCooldownTime                 = 60;
        time_t nextUnlockAttemptCooldownTime                        = initialCooldownTime;

        void setPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction);
        bool checkPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction);

        void setPhoneLockInSettings();
        void setPhoneLockAvailabilityInSettings(bool value);
        void savePhoneLockTime();
        void saveNextUnlockAttemptCooldownTime();
        void saveNoCooldownAttemptsLeft();
        void storePhoneLockInformation();

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
        void setPhoneLockTime(time_t time);
        void setNextUnlockAttemptCooldownTime(time_t time);
        void setNoCooldownAttemptsLeft(unsigned int attemptsNumber);
        [[nodiscard]] bool isPhoneLocked() const noexcept;
        void increaseLockTime() noexcept;
        void resetLockTime() noexcept;
    };

} // namespace locks

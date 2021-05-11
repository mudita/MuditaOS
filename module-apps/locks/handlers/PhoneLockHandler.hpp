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

        void setPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction);
        bool checkPhoneLockInputTypeAction(PhoneLockInputTypeAction _phoneLockInputTypeAction);

        void setPhoneLockInSettings();
        void setPhoneLockAvailabilityInSettings(bool value);

        void phoneLockAction();
        void phoneUnlockAction();
        void phoneLockEnableAction();
        void phoneLockDisableAction();
        void phoneLockChangeAction();
        void phoneLockSetAction();
        void phoneInputRequiredAction();
        void phoneUnlockPopupsCloseAction();
        void phoneLockChangeInfoAction();

        void checkNewPhoneLock();
        void resolvePhoneLockAction();

        sys::MessagePointer verifyPhoneUnlockInput(const std::vector<unsigned int> &inputData);
        sys::MessagePointer verifyPhoneLockChangeInput(const std::vector<unsigned int> &inputData);

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
        sys::MessagePointer verifyPhoneLockInput(const std::vector<unsigned int> &inputData);

        void enablePhoneLock(bool _phoneLockEnabled);
        void setPhoneLockHash(const std::string &value);
        [[nodiscard]] bool isPhoneLocked() const noexcept;
    };

} // namespace locks

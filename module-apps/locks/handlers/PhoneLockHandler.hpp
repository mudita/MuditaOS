// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/widgets/Lock.hpp>
#include <locks/data/LockMessages.hpp>

#include <module-sys/Service/Service.hpp>

namespace locks
{
    class PhoneLockHandler
    {
      private:
        sys::Service *owner;
        Lock lock;
        bool phoneLockEnabled      = true;
        unsigned int phoneLockHash = std::numeric_limits<unsigned>::max();

        void PhoneLockAction();
        void PhoneUnlockAction();
        void PhoneInputRequiredAction();
        void PhoneUnlockPopupsCloseAction();

      public:
        explicit PhoneLockHandler(sys::Service *owner);

        sys::MessagePointer handleLockRequest();
        sys::MessagePointer handleUnlockRequest();
        sys::MessagePointer handleUnlockCancelRequest();
        sys::MessagePointer verifyPhoneLockInput(const std::vector<unsigned int> &inputData);

        void enablePhoneLock(bool _phoneLockEnabled);
        void setPhoneLockHash(const std::string &value);
        [[nodiscard]] bool isPhoneLocked() const noexcept;
    };

} // namespace locks

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "locks/widgets/Lock.hpp"
#include "locks/data/LockMessages.hpp"

#include "module-sys/Service/Service.hpp"

namespace lock
{
    class PhoneLocHandler
    {
      private:
        Lock lock;

        using OnPhoneLockCallback          = std::function<void()>;
        using OnPhoneUnlockCallback        = std::function<void()>;
        using OnPhoneInputRequiredCallback = std::function<void(Lock)>;

        OnPhoneLockCallback onPhoneLockCallback;
        OnPhoneUnlockCallback onPhoneUnlockCallback;
        OnPhoneInputRequiredCallback onPhoneInputRequiredCallback;

        bool phoneLockEnabled      = true;
        unsigned int phoneLockHash = 0;

        void verifyCallbacks();

      public:
        PhoneLocHandler();

        void addOnPhoneLockCallback(OnPhoneLockCallback &&onLock) noexcept;
        void addOnPhoneUnlockCallback(OnPhoneUnlockCallback &&onUnlock) noexcept;
        void addOnPhoneInputRequiredCallback(OnPhoneInputRequiredCallback &&onInputRequired) noexcept;

        sys::MessagePointer handleLockRequest();
        sys::MessagePointer handleUnlockRequest();
        sys::MessagePointer verifyPhoneLockInput(const std::vector<unsigned int> &inputData);

        void enablePhoneLock(bool _phoneLockEnabled);
        void setPhoneLockHash(const std::string &value);
        [[nodiscard]] bool isPhoneLocked() const noexcept;
    };

} // namespace lock

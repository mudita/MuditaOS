// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/lock-widgets/Lock.hpp"
#include "popups/data/LockMessages.hpp"

#include "module-sys/Service/Service.hpp"

namespace lock
{
    class PhoneLockObserver
    {
      private:
        Lock *lock = nullptr;

        using OnPhoneLockCallback             = std::function<void()>;
        using OnPhoneUnlockCallback           = std::function<void()>;
        using OnPhonePasscodeRequiredCallback = std::function<void(Lock *)>;

        OnPhoneLockCallback onPhoneLockCallback;
        OnPhoneUnlockCallback onPhoneUnlockCallback;
        OnPhonePasscodeRequiredCallback onPhonePasscodeRequiredCallback;

        bool phoneLockEnabled      = true;
        unsigned int phoneLockHash = 0;

        sys::MessagePointer handleLockRequest();
        sys::MessagePointer handleUnlockRequest();

      public:
        PhoneLockObserver();
        ~PhoneLockObserver();

        void subscribe(OnPhoneLockCallback &&onLock,
                       OnPhoneUnlockCallback &&onUnlock,
                       OnPhonePasscodeRequiredCallback &&onPasscodeRequired) noexcept;

        void enablePhoneLock(bool _phoneLockEnabled);
        void setPhoneLockHash(const std::string &value);
        void connect(sys::Service *owner);
    };

} // namespace lock

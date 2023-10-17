// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/data/LockData.hpp>
#include <Service/Service.hpp>

namespace locks
{
    class PhoneLockSubject
    {
      private:
        sys::Service *owner;

      public:
        explicit PhoneLockSubject(sys::Service *owner);

        void unlock();
        void lock();
        void enablePhoneLock();
        void disablePhoneLock();
        void changePhoneLock();
        void setPhoneLock();
        void skipSetPhoneLock();
        void verifyInput(LockInput inputData);
    };

} // namespace locks

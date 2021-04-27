// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Service.hpp>

namespace lock
{
    class PhoneLockSubject
    {
      private:
        sys::Service *owner;

      public:
        explicit PhoneLockSubject(sys::Service *owner);

        void unlock();
        void lock();
        void verifyInput(const std::vector<unsigned int> &inputData);
    };

} // namespace lock

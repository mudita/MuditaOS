// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/data/LockData.hpp>

#include <Service/Service.hpp>
#include <service-cellular-api>

namespace locks
{
    class SimLockSubject
    {
      private:
        sys::Service *owner;

      public:
        explicit SimLockSubject(sys::Service *owner);

        void setSim(cellular::api::SimSlot simSlot);
        void resetSimLockState();
        void changeSimPin();
        void enableSimPin();
        void disableSimPin();
        void verifyInput(LockInput inputData);
    };

} // namespace locks

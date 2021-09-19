// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/data/LockData.hpp>

#include <Service/Service.hpp>
#include <service-cellular-api>
#include <EventStore.hpp>
#include <hal/cellular/SIM.hpp>

namespace locks
{
    class SimLockSubject
    {
      private:
        sys::Service *owner;

      public:
        explicit SimLockSubject(sys::Service *owner);

        void setSim(hal::cellular::SimSlot simSlot);
        void changeSimPin();
        void enableSimPin();
        void disableSimPin();
        void verifyInput(LockInput inputData);
    };

} // namespace locks

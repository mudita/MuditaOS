// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-db/queries/calendar/QueryEventsGet.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-sys/Service/Timer.hpp>
#include <functional>
#include <string> // for allocator, string

#include "Service/Service.hpp" // for Service
#include "ServiceTime.hpp"
#include "MessageType.hpp"
#include "timeEvents/CalendarTimeEvents.hpp" // for CalendarTimeEvents
#include "Service/Common.hpp"                // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp"               // for Message_t, DataMessage (ptr only), ResponseMessage (ptr only)
#include "service-time/ServiceTime.hpp"

namespace service::name
{
    inline constexpr auto service_time = "ServiceTime";
};

namespace stm
{
    class ServiceTime : public sys::Service
    {
      private:
        CalendarTimeEvents calendarEvents;

      public:
        ServiceTime();
        ~ServiceTime();

        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    };

} /* namespace stm */

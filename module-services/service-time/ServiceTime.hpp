// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-time/CalendarTimeEvents.hpp"
#include "service-time/AlarmsTimeEvents.hpp"
#include "service-time/ServiceTime.hpp"

#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-db/queries/calendar/QueryEventsGet.hpp>
#include <module-sys/Service/Timer.hpp>

#include <functional>
#include <string> // for allocator, string

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
        AlarmsTimeEvents alarmsEvents;

      public:
        ServiceTime();
        ~ServiceTime();

        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    };

} /* namespace stm */

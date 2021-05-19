// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"
#include "service-time/CalendarTimeEvents.hpp"
#include "service-time/TimeManager.hpp"
#include "service-time/ServiceTime.hpp"

#include <service-db/DBServiceName.hpp>

#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-db/queries/calendar/QueryEventsGet.hpp>

#include <functional>
#include <string> // for allocator, string

namespace stm
{
    class ServiceTime : public sys::Service
    {
      private:
        static constexpr auto StackDepth = 2048;
        CalendarTimeEvents calendarEvents;

        std::unique_ptr<TimeManager> timeManager;
        void registerMessageHandlers();

      public:
        ServiceTime();
        ~ServiceTime();

        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        void ProcessCloseReason(sys::CloseReason closeReason) override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    };

} /* namespace stm */

namespace sys
{
    template <> struct ManifestTraits<stm::ServiceTime>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::service_time;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys

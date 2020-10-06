#ifndef PUREPHONE_SERVICETIME_HPP
#define PUREPHONE_SERVICETIME_HPP

#include "Service/Service.hpp"
#include "ServiceTime.hpp"
#include <functional>
#include "MessageType.hpp"
#include "timeEvents/CalendarTimeEvents.hpp"

#include <module-db/queries/calendar/QueryEventsGet.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-sys/Service/Timer.hpp>

namespace service::name
{
    const inline std::string service_time = "ServiceTime";
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

        // Invoked during initialization
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    };

} /* namespace stm */

#endif // PUREPHONE_SERVICETIME_HPP

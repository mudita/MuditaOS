// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmEventsDBRepository.hpp"
#include "AlarmOperations.hpp"
#include "AlarmMessageHandler.hpp"
#include "Constants.hpp"
#include "service-time/TimeManager.hpp"
#include "service-time/ServiceTime.hpp"

#include <service-db/DBServiceName.hpp>

#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include <functional>
#include <string> // for allocator, string

namespace settings
{
    class Settings;
}

namespace stm
{
    class ServiceTime : public sys::Service
    {
      private:
        static constexpr auto StackDepth = 2048;

        std::unique_ptr<TimeManager> timeManager;

        std::unique_ptr<settings::Settings> settings;

        std::unique_ptr<alarms::AlarmMessageHandler> alarmMessageHandler;

        void registerMessageHandlers();
        auto handleSetAutomaticDateAndTimeRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleSetTimeFormatRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleSetDateFormatRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleSetTimezoneRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleCellularTimeNotificationMessage(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleGetAutomaticDateAndTimeRequest() -> std::shared_ptr<sys::ResponseMessage>;

        void initStaticData();

      public:
        ServiceTime(
            const alarms::IAlarmOperationsFactory &alarmOperationsFactory = alarms::CommonAlarmOperationsFactory{});
        ~ServiceTime();

        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        void ProcessCloseReason(sys::CloseReason closeReason) override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

        void addAlarmExecutionHandler(const alarms::AlarmType type,
                                      const std::shared_ptr<alarms::AlarmHandler> handler);
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

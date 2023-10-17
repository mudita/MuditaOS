// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Timers/TimerHandle.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <service-db/Settings.hpp>
#include <service-db/DBServiceName.hpp>

namespace service::name
{
    constexpr auto service_test = "service-test";
}

namespace service::test
{
    class ServiceTest : public sys::Service
    {
        settings::Settings settings;
        sys::TimerHandle th;
        bool appStarted = false;

      public:
        ServiceTest();
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    };
} // namespace service::test

namespace sys
{
    template <>
    struct ManifestTraits<service::test::ServiceTest>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::service_test;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys

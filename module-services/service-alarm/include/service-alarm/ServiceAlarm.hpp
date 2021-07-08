// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceAlarmImp.hpp"
#include <Service/Service.hpp>

namespace service::name
{
    constexpr inline auto alarm = "ServiceAlarm";
} // namespace service::name

class ServiceAlarm final : public sys::Service
{
  public:
    ServiceAlarm();

    ~ServiceAlarm();

    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    void ProcessCloseReason(sys::CloseReason closeReason) override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

  private:
    ServiceAlarmImp impl{this};
};

namespace sys
{
    template <> struct ManifestTraits<ServiceAlarm>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name = service::name::alarm;
            return manifest;
        }
    };
} // namespace sys

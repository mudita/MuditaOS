// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include <service-db/DBServiceName.hpp>

#include <memory>

namespace service::name
{
    constexpr inline auto lwip = "ServiceLwIP";
} // namespace service::name

class LwIP_message : public sys::DataMessage
{
  public:
    enum Request
    {
        None,
        Start,
        Stop,
    };
    enum Request req = Request::None;
    LwIP_message(enum Request req = None) : sys::DataMessage(MessageType::LwIP_request), req(req){};
    virtual ~LwIP_message() = default;
};

sys::ReturnCodes message_lwip(sys::Service *app, LwIP_message::Request req);

class ServiceLwIP : public sys::Service
{
    static constexpr auto StackDepth = 1024;

  public:
    ServiceLwIP();
    ~ServiceLwIP() = default;

    virtual sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    void ProcessCloseReason(sys::CloseReason closeReason) override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
};

namespace sys
{
    template <> struct ManifestTraits<ServiceLwIP>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::lwip;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys

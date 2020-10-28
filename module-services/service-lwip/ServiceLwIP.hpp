// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include "MessageType.hpp"     // for MessageType, MessageType::LwIP_request
#include "Service/Message.hpp" // for DataMessage, Message_t, ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Service/Common.hpp"  // for ReturnCodes, ServicePowerMode

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
  public:
    ServiceLwIP();
    ~ServiceLwIP() = default;

    virtual sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

  private:
    static const char *serviceName;
};

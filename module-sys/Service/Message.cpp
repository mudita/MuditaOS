// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Message.hpp"
#include "Service.hpp"

namespace sys
{
    SendResult CreateSendResult(ReturnCodes retCode, MessagePointer msg)
    {
        return std::make_pair(retCode, msg);
    }

    Message::Message(BusChannel channel) : channel{channel}
    {}

    MessagePointer Message::Execute(Service *service)
    {
        return Proxy::handleMessage(service, this);
    }

    SystemMessage::SystemMessage(SystemMessageType systemMessageType, ServicePowerMode pwrMode)
        : Message(BusChannel::System), systemMessageType(systemMessageType), powerMode(pwrMode)
    {
        type = Type::System;
    }

    MessagePointer SystemMessage::Execute(Service *service)
    {
        return Proxy::handleSystemMessage(service, this);
    }

    DataMessage::DataMessage(MessageType messageType) : messageType{messageType}
    {
        type = Type::Data;
    }

    DataMessage::DataMessage(BusChannel channel) : Message(channel)
    {
        type = Type::Data;
    }

    ResponseMessage::ResponseMessage(ReturnCodes retCode, MessageType responseTo)
        : retCode(retCode), responseTo(responseTo)
    {
        type = Type::Response;
    }

    MessagePointer ResponseMessage::Execute(Service *service)
    {
        return Proxy::handleMessage(service, nullptr, this);
    }
} // namespace sys

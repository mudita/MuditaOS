// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/Message.hpp>
#include <Service/Service.hpp>

namespace sys
{
    SendResult CreateSendResult(ReturnCodes retCode, MessagePointer msg)
    {
        return std::make_pair(retCode, msg);
    }

    MessageUIDType MessageUID::get() const noexcept
    {
        return value;
    }

    MessageUIDType MessageUID::getNext() noexcept
    {
        if (value == invalidMessageUid) {
            value = 0;
        }
        return value++;
    }

    Message::Message(Type type) : type(type)
    {}

    Message::Message(Type type, BusChannel channel) : type(type), channel{channel}
    {}

    MessagePointer Message::Execute(Service *service)
    {
        return Proxy::handleMessage(service, this);
    }

    bool Message::ValidateMessage() const noexcept
    {
        return !(id == invalidMessageUid || type == Message::Type::Unspecified || sender == "Unknown");
    }

    void Message::ValidateUnicastMessage() const
    {
        if (!ValidateMessage() || uniID == invalidMessageUid || transType != Message::TransmissionType::Unicast) {
            throw std::runtime_error("Message unicast lack essential fields");
        }
    }

    void Message::ValidateResponseMessage() const
    {
        if (!ValidateMessage()) {
            throw std::runtime_error("Message response lack essential fields");
        }
    }

    void Message::ValidateBroadcastMessage() const
    {
        if (!ValidateMessage() || transType != Message::TransmissionType::Broadcast) {
            throw std::runtime_error("Message broadcast lack essential fields");
        }
    }

    void Message::ValidateMulticastMessage() const
    {
        if (!ValidateMessage() || channel == BusChannel::Unknown || transType != Message::TransmissionType::Multicast) {
            throw std::runtime_error("Message multicast lack essential fields");
        }
    }

    SystemMessage::SystemMessage(SystemMessageType systemMessageType, ServicePowerMode pwrMode)
        : Message(Type::System, BusChannel::System), systemMessageType(systemMessageType), powerMode(pwrMode)
    {}

    MessagePointer SystemMessage::Execute(Service *service)
    {
        return Proxy::handleSystemMessage(service, this);
    }

    ServiceCloseReasonMessage::ServiceCloseReasonMessage(CloseReason closeReason)
        : SystemMessage(SystemMessageType::ServiceCloseReason), closeReason(closeReason)
    {}

    CloseReason ServiceCloseReasonMessage::getCloseReason() const noexcept
    {
        return closeReason;
    }

    DataMessage::DataMessage(MessageType messageType) : Message(Type::Data), messageType{messageType}
    {}

    DataMessage::DataMessage(BusChannel channel) : Message(Type::Data, channel)
    {}

    DataMessage::DataMessage() : Message(Type::Data)
    {}

    ResponseMessage::ResponseMessage(ReturnCodes retCode, MessageType responseTo)
        : Message(Type::Response), retCode(retCode), responseTo(responseTo)
    {}

    MessagePointer ResponseMessage::Execute(Service *service)
    {
        return Proxy::handleMessage(service, nullptr, this);
    }

} // namespace sys

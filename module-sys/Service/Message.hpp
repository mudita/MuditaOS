// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include "MessageType.hpp"
#include "MessageForward.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace sys
{
    SendResult CreateSendResult(ReturnCodes retCode, MessagePointer msg);

    class Message
    {
      public:
        enum class TransmissionType
        {
            Unicast,
            Multicast,
            Broadcast
        };
        enum class Type
        {
            System,
            Data,
            Response
        };

        Message() = default;
        explicit Message(BusChannel channel);
        virtual ~Message() noexcept = default;

        virtual MessagePointer Execute(Service *service);

        virtual operator std::string() const
        {
            return {"{}"};
        }

        std::uint64_t id;
        std::uint64_t uniID;
        Type type;
        TransmissionType transType;
        BusChannel channel;
        std::string sender;
    };

    enum class SystemMessageType
    {
        Ping,
        SwitchPowerMode,
        Start,
        Timer,
        Exit
    };

    class SystemMessage : public Message
    {
      public:
        explicit SystemMessage(SystemMessageType systemMessageType,
                               ServicePowerMode pwrMode = ServicePowerMode::Active);

        MessagePointer Execute(Service *service) final;

        SystemMessageType systemMessageType;
        ServicePowerMode powerMode;
    };

    class ServiceCloseReasonMessage : public Message
    {
      public:
        explicit ServiceCloseReasonMessage(CloseReason closeReason);

        MessagePointer Execute(Service *service) final;

        CloseReason getCloseReason() const noexcept;

      private:
        const CloseReason closeReason;
    };

    class ReadyToCloseMessage : public Message
    {};

    class DataMessage : public Message
    {
      public:
        explicit DataMessage(MessageType messageType);
        explicit DataMessage(BusChannel channel);

        // This field must by provided by the class that inherits DataMessage
        MessageType messageType = MessageType::MessageTypeUninitialized;
    };

    class ResponseMessage : public Message
    {
      public:
        explicit ResponseMessage(ReturnCodes retCode    = ReturnCodes::Success,
                                 MessageType responseTo = MessageType::MessageTypeUninitialized);

        MessagePointer Execute(Service *service) final;

        ReturnCodes retCode;
        MessageType responseTo;
    };
} // namespace sys

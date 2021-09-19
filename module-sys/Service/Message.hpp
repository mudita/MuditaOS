// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include "MessageForward.hpp"

// TODO
#include <source/MessageType.hpp>
#include <magic_enum.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace sys
{
    SendResult CreateSendResult(ReturnCodes retCode, MessagePointer msg);

    inline constexpr std::uint64_t invalidMessageUid = std::numeric_limits<uint64_t>().max();

    using MessageUIDType = std::uint64_t;

    class MessageUID
    {
      protected:
        MessageUIDType value = 0;

      public:
        [[nodiscard]] MessageUIDType get() const noexcept;
        [[nodiscard]] MessageUIDType getNext() noexcept;
    };

    class Message
    {
      public:
        enum class TransmissionType
        {
            Unspecified,
            Unicast,
            Multicast,
            Broadcast
        };

        enum class Type
        {
            Unspecified,
            System,
            Data,
            Response
        };

        explicit Message(Type type);
        Message(Type type, BusChannel channel);
        virtual ~Message() noexcept = default;

        virtual MessagePointer Execute(Service *service);

        virtual explicit operator std::string() const
        {
            return {"{}"};
        }

        MessageUIDType id          = invalidMessageUid;
        MessageUIDType uniID       = invalidMessageUid;
        Type type                  = Type::Unspecified;
        TransmissionType transType = TransmissionType::Unspecified;
        BusChannel channel         = BusChannel::Unknown;
        std::string sender         = "Unknown";

        [[nodiscard]] std::string to_string() const
        {
            return "| ID:" + std::to_string(id) + " | uniID: " + std::to_string(uniID) +
                   " | Type: " + std::string(magic_enum::enum_name(type)) +
                   " | TransmissionType: " + std::string(magic_enum::enum_name(transType)) +
                   " | Channel: " + std::string(magic_enum::enum_name(channel)) + " | Sender: " + sender + " |";
        }

        /**
         * Validate base message have all essential fields set.
         * @return Return validation result.
         */
        [[nodiscard]] bool ValidateMessage() const noexcept;

        /**
         * Validate if response message have all essential fields set.
         * @return Return validation result.
         */
        void ValidateResponseMessage() const;

        /**
         * Validate if message sent via Unicast have all essential fields set.
         * @return Return validation result.
         */
        void ValidateUnicastMessage() const;

        /**
         * Validate if message sent via Broadcast have all essential fields set.
         * @return Return validation result.
         */
        void ValidateBroadcastMessage() const;

        /**
         * Validate if message sent via Multicast have all essential fields set.
         * @return Return validation result.
         */
        void ValidateMulticastMessage() const;
    };

    namespace msg
    {
        struct Request : public sys::Message
        {
            Request() : Message(Type::Data)
            {}
            virtual const char *target() const = 0;
        };

        struct Response : public sys::Message
        {
            explicit Response(bool retCode = true) : Message(Type::Data), retCode(retCode)
            {}
            const bool retCode;
        };

        struct Notification : public sys::Message
        {
            Notification() : Message(Type::Data)
            {}
            virtual sys::BusChannel channel() const = 0;
        };
    } // namespace msg

    enum class SystemMessageType
    {
        Ping,
        SwitchPowerMode,
        Start,
        Timer,
        Exit,
        ServiceCloseReason
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

    class ServiceCloseReasonMessage : public SystemMessage
    {
      public:
        explicit ServiceCloseReasonMessage(CloseReason closeReason);

        [[nodiscard]] CloseReason getCloseReason() const noexcept;

      private:
        const CloseReason closeReason;
    };

    class DataMessage : public Message
    {
      public:
        explicit DataMessage(MessageType messageType);
        explicit DataMessage(BusChannel channel);
        DataMessage();

        // This field must by provided by the class that inherits DataMessage
        MessageType messageType = MessageType::MessageTypeUninitialized;
    };

    class ReadyToCloseMessage : public DataMessage
    {};

    class ResponseMessage : public Message
    {
      public:
        explicit ResponseMessage(ReturnCodes retCode    = ReturnCodes::Success,
                                 MessageType responseTo = MessageType::MessageTypeUninitialized);

        MessagePointer Execute(Service *service) final;

        ReturnCodes retCode;
        MessageType responseTo;
    };

    inline auto msgHandled() -> MessagePointer
    {
        return std::make_shared<ResponseMessage>();
    }

    inline auto msgNotHandled() -> MessagePointer
    {
        return std::make_shared<ResponseMessage>(ReturnCodes::Unresolved);
    }
} // namespace sys

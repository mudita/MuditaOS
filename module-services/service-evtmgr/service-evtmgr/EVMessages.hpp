// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "KbdMessage.hpp"
#include "BatteryMessages.hpp"
#include "LightControl.hpp"

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <SwitchData.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <bsp/torch/torch.hpp>

#include <string>

namespace sevm
{
    namespace message
    {
        class GPIO : public Message
        {
          public:
            GPIO() : Message(MessageType::EVM_GPIO)
            {}
            uint32_t num = 0, port = 0, state = 0;
        };
    } // namespace message

    class RtcMinuteAlarmMessage : public Message
    {
      public:
        RtcMinuteAlarmMessage(MessageType messageType) : Message(messageType)
        {
            type = Type::Data;
        }
        uint32_t timestamp = 0;
    };

    class SIMMessage : public sys::DataMessage
    {
      public:
        SIMMessage() : DataMessage(MessageType::SIMTrayEvent)
        {
            type = Type::Data;
        }
    };

    /*
     * @brief Template for all messages that go to application manager
     */
    class EVMMessage : public sys::DataMessage
    {
      public:
        EVMMessage(MessageType messageType) : sys::DataMessage(messageType){};
        ~EVMMessage() override = default;
    };

    class EVMFocusApplication : public EVMMessage
    {
        std::string application;

      public:
        EVMFocusApplication(const std::string application)
            : EVMMessage(MessageType::EVMFocusApplication), application{application}
        {}
        [[nodiscard]] auto getApplication() const -> const std::string &
        {
            return application;
        };
    };

    class EVMAlarmSwitchData : public gui::SwitchData
    {
      public:
        EVMAlarmSwitchData() = default;
        EVMAlarmSwitchData(uint32_t id) : dbID(id){};
        ~EVMAlarmSwitchData() override = default;
        uint32_t dbID                  = 0;
    };

    class EVMBoardResponseMessage : public sys::ResponseMessage
    {
      public:
        EVMBoardResponseMessage(bool retCode,
                                std::string retdata    = std::string(),
                                MessageType responseTo = MessageType::MessageTypeUninitialized)
            : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode){};
        EVMBoardResponseMessage(bool retCode, MessageType responseTo)
            : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode){};

        bool retCode;
        bsp::Board board = bsp::Board::none;
    };

    class StatusStateMessage : public Message
    {
      public:
        StatusStateMessage(MessageType messageType) : Message(messageType)
        {
            type = Type::Data;
        }
        bsp::cellular::status::value state = bsp::cellular::status::value::INACTIVE;
    };

    class TorchStateMessage : public Message
    {
      public:
        TorchStateMessage(bsp::torch::Action direction) : Message(MessageType::EVMTorchStateMessage), action(direction)
        {}
        bsp::torch::Action action;
        bsp::torch::State state                  = bsp::torch::State::off;
        bsp::torch::ColourTemperature colourTemp = bsp::torch::ColourTemperature::no_change;
    };

    class TorchStateResultMessage : public TorchStateMessage
    {
      public:
        TorchStateResultMessage(bsp::torch::Action direction) : TorchStateMessage(direction)
        {}
        bool success = false;
    };
    class LightControlMessage : public Message
    {
      public:
        LightControlMessage() : Message(MessageType::EVMLightControlMessage)
        {}

        light_control::LightControlAction action;
        unsigned int value;
    };

    class LightControlResponseMessage : public LightControlMessage
    {
      public:
        explicit LightControlResponseMessage(bool status) : LightControlMessage(), success(status)
        {}
        bool success;
    };

} /* namespace sevm*/

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "KbdMessage.hpp"
#include "BatteryMessages.hpp"

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <SwitchData.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <vibra/Vibra.hpp>

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

    class RtcUpdateTimeMessage : public sys::Message
    {
      public:
        explicit RtcUpdateTimeMessage(time_t time) : time(time)
        {}
        [[nodiscard]] time_t getTime() const noexcept
        {
            return time;
        }

      private:
        const time_t time = 0;
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

    class ToggleTorchOnOffMessage : public Message
    {};

    class ToggleTorchColorMessage : public Message
    {};

    class KeypadBacklightMessage : public Message
    {
      public:
        explicit KeypadBacklightMessage(bsp::keypad_backlight::Action action) : action(action)
        {}

        bsp::keypad_backlight::Action action;
    };

    class KeypadBacklightResponseMessage : public Message
    {
      public:
        KeypadBacklightResponseMessage()
        {}
        bool success;
    };

    class VibraMessage : public Message
    {
      public:
        VibraMessage(bsp::vibrator::Action act,
                     std::chrono::milliseconds rptTime = std::chrono::milliseconds{bsp::vibrator::defaultVibraPauseMs})
            : Message(MessageType::VibraPulseMessage), action(act), repetitionTime(rptTime)
        {}

        bsp::vibrator::Action action;
        std::chrono::milliseconds repetitionTime;
    };

} /* namespace sevm*/

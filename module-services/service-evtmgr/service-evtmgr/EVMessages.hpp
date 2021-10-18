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
#include <hal/key_input/KeyEventDefinitions.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <vibra/Vibra.hpp>

#include <string>

namespace sevm
{
    namespace message
    {
        class GPIO : public sys::DataMessage
        {
          public:
            GPIO() : DataMessage(MessageType::EVM_GPIO)
            {}
            uint32_t num = 0, port = 0, state = 0;
        };
    } // namespace message

    class RtcMinuteAlarmMessage : public sys::DataMessage
    {
      public:
        RtcMinuteAlarmMessage(MessageType messageType) : DataMessage(messageType)
        {}
        uint32_t timestamp = 0;
    };

    class SIMTrayMessage : public sys::DataMessage
    {
      public:
        SIMTrayMessage() : DataMessage(MessageType::SIMTrayEvent)
        {}
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

    class StatusStateMessage : public sys::DataMessage
    {
      public:
        explicit StatusStateMessage(MessageType messageType) : DataMessage(messageType)
        {}
        bsp::cellular::status::value state = bsp::cellular::status::value::INACTIVE;
    };

    class ToggleTorchOnOffMessage : public sys::DataMessage
    {};

    class ToggleTorchColorMessage : public sys::DataMessage
    {};

    class KeypadBacklightMessage : public sys::DataMessage
    {
      public:
        explicit KeypadBacklightMessage(bsp::keypad_backlight::Action action) : action(action)
        {}

        bsp::keypad_backlight::Action action;
    };

    class KeypadBacklightResponseMessage : public sys::DataMessage
    {
      public:
        bool success;
    };

    class VibratorMessage : public sys::DataMessage
    {
      public:
        explicit VibratorMessage(
            bsp::vibrator::Action act,
            std::chrono::milliseconds rptTime = std::chrono::milliseconds{bsp::vibrator::defaultVibraPauseMs})
            : DataMessage(MessageType::VibratorPulseMessage), action(act), repetitionTime(rptTime)
        {}

        bsp::vibrator::Action action;
        std::chrono::milliseconds repetitionTime;
    };

    class VibratorLevelMessage : public sys::DataMessage
    {
      public:
        explicit VibratorLevelMessage(unsigned int vibrationLevel)
            : DataMessage(MessageType::VibratorLevelMessage), vibrationLevel(vibrationLevel)
        {}

        const unsigned int vibrationLevel;
    };

    class FlushLogsRequest : public sys::DataMessage
    {};

    class FlushLogsResponse : public sys::ResponseMessage
    {
      public:
        FlushLogsResponse(bool retCode, int retData = 0)
            : sys::ResponseMessage(sys::ReturnCodes::Success, MessageType::MessageTypeUninitialized), retCode(retCode),
              data(retData){};

        const bool retCode{};
        const int data{};
    };

} /* namespace sevm*/

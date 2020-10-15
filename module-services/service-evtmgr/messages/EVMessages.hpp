#ifndef MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_
#define MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_

#include <string>
#include <Service/Message.hpp>
#include <MessageType.hpp>
#include <SwitchData.hpp>
#include <utility>
#include "Service/Service.hpp"
#include "bsp/keyboard/key_codes.hpp"
#include "bsp/common.hpp"
#include "KbdMessage.hpp"
#include "BatteryMessages.hpp"
#include "bsp/cellular/bsp_cellular.hpp"
#include "bsp/torch/torch.hpp"
#include "bsp/vibrator/vibrator.hpp"

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

    class VibratorStateMessage : public Message, public bsp::vibrator::Vibration
    {
      public:
        VibratorStateMessage(bsp::vibrator::State state, std::optional<uint16_t> duration = std::nullopt)
            : Message(MessageType::EVMVibratorStateMessage), Vibration(state, duration)
        {}
    };

    class VibratorStateSequenceMessage : public Message
    {
      public:
        VibratorStateSequenceMessage(std::deque<bsp::vibrator::Vibration> vibrationSequence)
            : Message(MessageType::EVMVibratorStateMessage), vibrSeq(std::move(vibrationSequence))
        {}
        std::deque<bsp::vibrator::Vibration> vibrSeq;
    };
} /* namespace sevm*/

#endif /* MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_ */

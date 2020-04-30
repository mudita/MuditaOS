#ifndef MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_
#define MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_

#include <string>
#include <Service/Message.hpp>
#include <MessageType.hpp>
#include <SwitchData.hpp>
#include "Service/Service.hpp"
#include "bsp/keyboard/key_codes.hpp"
#include "KbdMessage.hpp"
#include "BatteryMessages.hpp"

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

    class EVMResponseMessage : public sys::ResponseMessage
    {
      public:
        EVMResponseMessage(bool retCode,
                           std::string retdata    = std::string(),
                           MessageType responseTo = MessageType::MessageTypeUninitialized)
            : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode){};
        EVMResponseMessage(bool retCode, MessageType responseTo)
            : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode){};
        virtual ~EVMResponseMessage(){};

        bool retCode;
    };
    class StateMessage : public Message
    {
      public:
        StateMessage(MessageType messageType) : Message(messageType)
        {
            type = Type::Data;
        }
        bool state = false;
    };
} /* namespace sevm*/

#endif /* MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_ */

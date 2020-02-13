/*
 * @file EVMessages.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 17 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_
#define MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_

#include <string>
#include "Service/Message.hpp"
#include "MessageType.hpp"
#include "SwitchData.hpp"
#include "Service/Service.hpp"
#include "MessageType.hpp"
#include "bsp/keyboard/key_codes.hpp"
#include "common_data/RawKey.hpp"


namespace sevm {

    struct Message : public sys::DataMessage
    {
        Message(MessageType messageType) : DataMessage(messageType)
        {
        }
        auto Execute(sys::Service *service) -> sys::Message_t override
        {
            // Ignore incoming data message if this service is not yet initialized
            if (service->isReady)
            {
                return service->DataReceivedHandler(this, nullptr);
            }
            else
            {
                return std::make_shared<sys::ResponseMessage>();
            }
        }
    };

    class KbdMessage : public Message
    {
      public:
        KbdMessage() : Message(MessageType::KBDKeyEvent)
        {
            type = Type::Data;
        }
        RawKey key = {};
    };

    namespace message
    {
        class GPIO : public Message
        {
          public:
            GPIO() : Message(MessageType::EVM_GPIO)
            {
            }
            uint32_t num = 0, port = 0, state = 0;
        };
    } // namespace message

    class BatteryLevelMessage : public Message
    {
      public:
        BatteryLevelMessage(MessageType messageType) : Message(messageType)
        {
            type = Type::Data;
        }
        uint8_t levelPercents = 0;
        bool fullyCharged = false;
};

class BatteryPlugMessage : public Message
{
public:
  BatteryPlugMessage(MessageType messageType) : Message(messageType)
  {
      type = Type::Data;
	}
	bool plugged = false;
};

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
class EVMMessage: public sys::DataMessage {
public:
  EVMMessage(MessageType messageType) : sys::DataMessage(messageType){};
  ~EVMMessage() override = default;
};

class EVMFocusApplication : public EVMMessage {
	std::string application;
public:
	EVMFocusApplication( const std::string application ) :
		EVMMessage( MessageType::EVMFocusApplication),
		application{ application } {
	}
    [[nodiscard]] auto getApplication() const -> const std::string &
    {
        return application;
    };
};

class EVMAlarmSwitchData :public gui::SwitchData
{
public:
  EVMAlarmSwitchData() = default;
  EVMAlarmSwitchData(uint32_t id) : dbID(id){};
  ~EVMAlarmSwitchData() override = default;
  uint32_t dbID = 0;
};
} /* namespace sevm*/

#endif /* MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_ */

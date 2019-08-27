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


namespace sevm {

enum class KeyboardEvents{
	keyPressed,
	keyReleasedShort,
	keyReleasedLong
};


class KbdMessage : public sys::DataMessage
{
public:
	KbdMessage(MessageType messageType) : DataMessage(static_cast<uint32_t>(messageType))
	{
		type = Type::Data;

	}

	sys::Message_t Execute(sys::Service* service)
	{
		// Ignore incoming data message if this service is not yet initialized
		if(service->isReady){
			return service->DataReceivedHandler(this,nullptr);
		}
		else{
			return std::make_shared<sys::ResponseMessage>();
		}

	}

	KeyboardEvents keyState = static_cast< KeyboardEvents>(0);
	bsp::KeyCodes keyCode = static_cast<bsp::KeyCodes>(0);
	int keyPressTime = 0;
	int keyRelaseTime = 0;
};

class BatteryLevelMessage : public sys::DataMessage
{
public:
	BatteryLevelMessage(MessageType messageType) : DataMessage(static_cast<uint32_t>(messageType))
	{
		type = Type::Data;

	}

	sys::Message_t Execute(sys::Service* service)
	{
		// Ignore incoming data message if this service is not yet initialized
		if(service->isReady){
			return service->DataReceivedHandler(this,nullptr);
		}
		else{
			return std::make_shared<sys::ResponseMessage>();
		}

	}
	uint8_t levelPercents = 0;
	bool fullyCharged = false;
};

class BatteryPlugMessage : public sys::DataMessage
{
public:
	BatteryPlugMessage(MessageType messageType) : DataMessage(static_cast<uint32_t>(messageType))
	{
		type = Type::Data;

	}

	sys::Message_t Execute(sys::Service* service)
	{
		// Ignore incoming data message if this service is not yet initialized
		if(service->isReady){
			return service->DataReceivedHandler(this,nullptr);
		}
		else{
			return std::make_shared<sys::ResponseMessage>();
		}

	}
	bool plugged = false;
};

class RtcMinuteAlarmMessage : public sys::DataMessage
{
public:
	RtcMinuteAlarmMessage(MessageType messageType) : DataMessage(static_cast<uint32_t>(messageType))
	{
		type = Type::Data;

	}
	sys::Message_t Execute(sys::Service* service)
	{
		// Ignore incoming data message if this service is not yet initialized
		if(service->isReady){
			return service->DataReceivedHandler(this,nullptr);
		}
		else{
			return std::make_shared<sys::ResponseMessage>();
		}

	}
	uint32_t timestamp = 0;
};
/*
 * @brief Template for all messages that go to application manager
 */
class EVMMessage: public sys::DataMessage {
public:
	EVMMessage( MessageType messageType ) :
		sys::DataMessage( static_cast<uint32_t>(messageType)) {};
	virtual ~EVMMessage() {};
};

class EVMFocusApplication : public EVMMessage {
	std::string application;
public:
	EVMFocusApplication( const std::string application ) :
		EVMMessage( MessageType::EVMFocusApplication),
		application{ application } {
	}
	const std::string& getApplication() const { return application; };
};

class EVMAlarmSwitchData :public gui::SwitchData
{
public:
	EVMAlarmSwitchData() {};
	EVMAlarmSwitchData(uint32_t id) : dbID(id) {};
	~EVMAlarmSwitchData() {};
	uint32_t dbID = 0;
};
} /* namespace sevm*/

#endif /* MODULE_SERVICES_SERVICE_EVTMGR_MESSAGES_EVMESSAGES_HPP_ */

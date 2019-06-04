/*
 * EventManager.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_KBD_EVENTMANAGER_HPP_
#define MODULE_SERVICES_SERVICE_KBD_EVENTMANAGER_HPP_

#include "module-bsp/bsp/keyboard/key_codes.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"
#include "MessageType.hpp"

#include "common.hpp"

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
	sys::Message_t Execute(sys::Service* service) override;

	KeyboardEvents keyState = static_cast< KeyboardEvents>(0);
	bsp::KeyCodes keyCode = static_cast<bsp::KeyCodes>(0);
	int keyPressTime = 0;
	int keyRelaseTime = 0;
};

class EventManager: public sys::Service {
protected:
	sys::Worker* EventWorker;
public:
	EventManager(const std::string& name);
    ~EventManager();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

    uint32_t timer_id= 0;
};

#endif /* MODULE_SERVICES_SERVICE_KBD_EventManager_HPP_ */

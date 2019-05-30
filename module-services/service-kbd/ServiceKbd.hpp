/*
 * ServiceKbd.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_KBD_SERVICEKBD_HPP_
#define MODULE_SERVICES_SERVICE_KBD_SERVICEKBD_HPP_

#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "key_codes_linux.hpp"
#include "common.hpp"


class KbdMessage : public sys::DataMessage
{
public:
	KbdMessage()
	{
		type = Type::Data;

	}
	KbdMessage(sys::BusChannels cannel) : sys::DataMessage(channel)
	{
		type = Type::Data;
	}
	sys::Message_t Execute(sys::Service* service) override;

	bsp::KeyEvents keyState = static_cast<bsp::KeyEvents>(0);
	bsp::KeyCodes keyCode = static_cast<bsp::KeyCodes>(0);
	int keyPressTime = 0;
	int keyRelaseTime = 0;
};

class ServiceKbd: public sys::Service {
public:
	ServiceKbd(const std::string& name);
    ~ServiceKbd();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

};

#endif /* MODULE_SERVICES_SERVICE_KBD_SERVICEKBD_HPP_ */

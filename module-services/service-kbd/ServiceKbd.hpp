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
#include "Service/Worker.hpp"

class ServiceKbd: public sys::Service {
protected:
	sys::Worker* kbdWorker;
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

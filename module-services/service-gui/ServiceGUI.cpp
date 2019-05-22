/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "ServiceGUI.hpp"
#include "log/log.hpp"

ServiceGUI::ServiceGUI(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("ServiceGUI Allocating resources");
}

ServiceGUI::~ServiceGUI(){
	LOG_INFO("ServiceGUI Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceGUI::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceGUI::InitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceGUI::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceGUI::SleepHandler() {
	return sys::ReturnCodes::Success;
}





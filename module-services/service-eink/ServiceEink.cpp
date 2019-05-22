/*
 * ServiceEink.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "ServiceEink.hpp"

#include "log/log.hpp"

ServiceEink::ServiceEink(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[ServiceEink] Initializing");
}

ServiceEink::~ServiceEink(){
	LOG_INFO("[ServiceEink] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceEink::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceEink::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceEink::InitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceEink::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceEink::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceEink::SleepHandler() {
	return sys::ReturnCodes::Success;
}





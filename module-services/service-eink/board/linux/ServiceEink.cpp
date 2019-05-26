/*
 * ServiceEink.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */


//module-utils
#include "log/log.hpp"

#include "ServiceEink.hpp"

ServiceEink::ServiceEink(const std::string& name)
	: sys::Service(name),
	  timerID { 0 },
	  selfRefereshTriggerCount{ 0 },
	  temperatureMeasurementTriggerCount{ 0 },
	  powerOffTriggerCount{ 0 }
{
	LOG_INFO("[ServiceEink] Initializing");

	//deepClearScreen( 24 );

	timerID = CreateTimer(1000,true);
	ReloadTimer(timerID);
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


bool ServiceEink::deepClearScreen(int8_t temperature)
{

    return true;
}





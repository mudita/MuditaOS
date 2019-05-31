/*
 * ServiceKbd.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "ServiceKbd.hpp"

#include "log/log.hpp"

ServiceKbd::ServiceKbd(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[ServiceKbd] Initializing");
}

ServiceKbd::~ServiceKbd(){
	LOG_INFO("[ServiceKbd] Cleaning resources");
	if( kbdWorker != nullptr) {
		kbdWorker->deinit();
		delete kbdWorker;
	}
}

// Invoked upon receiving data message
sys::Message_t ServiceKbd::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceKbd::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceKbd::InitHandler() {
	kbdWorker = new sys::Worker( this );
	sys::WorkerQueueInfo test1 = {"aaaa", 10, 10 };
	std::list<sys::WorkerQueueInfo> list;
	list.push_back(test1);
	kbdWorker->init( list );
	kbdWorker->run();
	vTaskDelay(1000);
	kbdWorker->send( 123, nullptr );

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceKbd::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceKbd::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceKbd::SleepHandler() {
	return sys::ReturnCodes::Success;
}





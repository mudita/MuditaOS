/*
 * ServiceKbd.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "ServiceKbd.hpp"

#include "log/log.hpp"

#include "keyboard/keyboard.hpp"
#include "linux_keyboard.hpp"




sys::Message_t KbdMessage::Execute(sys::Service* service)
{
	// Ignore incoming data message if this service is not yet initialized
	if(service->isReady){
		return service->DataReceivedHandler(this);
	}
	else{
		return std::make_shared<sys::ResponseMessage>();
	}

}

ServiceKbd::ServiceKbd(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[ServiceKbd] Initializing");
	bsp::keyboard keyboard;
	keyboard.Init(bsp::linux_keyboard_event_callback, this);
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

	KbdMessage* msg = static_cast<KbdMessage*>(msgl);

	LOG_INFO("[ServiceKbd] Received key info: key_code = %d, keyEvent = %d\n"
			"press time: %d, release time %d", static_cast<int>(msg->keyCode),
			static_cast<int>(msg->keyState), msg->keyPressTime, msg->keyRelaseTime);

	///TEST long press always active


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





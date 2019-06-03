/*
 * ServiceKbd.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "ServiceKbd.hpp"

#include "../../module-bsp/board/linux/keyboard/bsp_keyboard.hpp"
#include "log/log.hpp"

#include "keyboard/keyboard.hpp"
#include "WorkerKbd.hpp"



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


}

ServiceKbd::~ServiceKbd(){

    timer_id = CreateTimer(1000,true);
    ReloadTimer(timer_id);


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
	kbdWorker->send( 123, nullptr );
	///TEST long press always active


	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void ServiceKbd::TickHandler(uint32_t id) {


	LOG_INFO("[ServiceKbd] send to worker\n");

}

// Invoked during initialization
sys::ReturnCodes ServiceKbd::InitHandler() {



	//initialize keyboard worker
	kbdWorker = new WorkerKbd( this );

	//create queues for worker
	sys::WorkerQueueInfo qTimer = {"qTimer", sizeof(bool), 10 };
	sys::WorkerQueueInfo qIrq = {"qIrq", sizeof(bsp::KeyState), 10 };
	std::list<sys::WorkerQueueInfo> list;

	list.push_back(qTimer);
	list.push_back(qIrq);

	kbdWorker->init( list );
	kbdWorker->run();

	std::vector<xQueueHandle> set = kbdWorker->getQueues();

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





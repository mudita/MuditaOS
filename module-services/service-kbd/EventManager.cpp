/*
 * EventManager.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "EventManager.hpp"

#include "module-bsp/board/linux/keyboard/bsp_keyboard.hpp"
#include "log/log.hpp"

#include "keyboard/keyboard.hpp"
#include "WorkerEvent.hpp"



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

EventManager::EventManager(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[EventManager] Initializing");


}

EventManager::~EventManager(){

    timer_id = CreateTimer(1000,true);
    ReloadTimer(timer_id);


	LOG_INFO("[EventManager] Cleaning resources");
	if( EventWorker != nullptr) {
		EventWorker->deinit();
		delete EventWorker;
	}
}

// Invoked upon receiving data message
sys::Message_t EventManager::DataReceivedHandler(sys::DataMessage* msgl) {

	KbdMessage* msg = static_cast<KbdMessage*>(msgl);

	LOG_INFO("[EventManager] Received key info: key_code = %d, keyEvent = %d\n"
			"press time: %d, release time %d", static_cast<int>(msg->keyCode),
			static_cast<int>(msg->keyState), msg->keyPressTime, msg->keyRelaseTime);

	if( (msg->keyCode == bsp::KeyCodes::NumericKeyAst) || (msg->keyCode == bsp::KeyCodes::NumericKeyPnd))
	{
		auto message = std::make_shared<KbdMessage>(MessageType::KBDKeyEvent);
		message->keyCode = msg->keyCode;
		message->keyState = msg->keyState;

		sys::Bus::SendUnicast(message, "ApplicationClock", this);
	}
	return std::make_shared<sys::ResponseMessage>();
}

// Invoked when timer ticked
void EventManager::TickHandler(uint32_t id) {


	LOG_INFO("[EventManager] send to worker\n");

}

// Invoked during initialization
sys::ReturnCodes EventManager::InitHandler() {

	//initialize keyboard worker
	EventWorker = new WorkerEvent( this );

	//create queues for worker
	sys::WorkerQueueInfo qTimer = {"qTimer", sizeof(bool), 10 };
	sys::WorkerQueueInfo qIrq = {"qIrq", sizeof(KeyState), 10 };
	std::list<sys::WorkerQueueInfo> list;

	list.push_back(qTimer);
	list.push_back(qIrq);

	EventWorker->init( list );
	EventWorker->run();

	std::vector<xQueueHandle> set = EventWorker->getQueues();

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes EventManager::SleepHandler() {
	return sys::ReturnCodes::Success;
}





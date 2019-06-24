/*
 * EventManager.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "EventManager.hpp"

#include "log/log.hpp"

#include "keyboard/keyboard.hpp"
#include "WorkerEvent.hpp"
#include "messages/EVMessages.hpp"


EventManager::EventManager(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[EventManager] Initializing");


}

EventManager::~EventManager(){

	LOG_INFO("[EventManager] Cleaning resources");
	if( EventWorker != nullptr) {
		EventWorker->deinit();
		delete EventWorker;
	}
}

// Invoked upon receiving data message
sys::Message_t EventManager::DataReceivedHandler(sys::DataMessage* msgl) {

	bool handled = false;

	if(msgl->messageType == static_cast<uint32_t>(MessageType::KBDKeyEvent) &&
		msgl->sender == this->GetName()) {
		sevm::KbdMessage* msg = reinterpret_cast<sevm::KbdMessage*>(msgl);

		LOG_INFO("[EventManager] Received key info: key_code = %d, keyEvent = %d\n"
				"press time: %d, release time %d", static_cast<int>(msg->keyCode),
				static_cast<int>(msg->keyState), msg->keyPressTime, msg->keyRelaseTime);

		auto message = std::make_shared<sevm::KbdMessage>(MessageType::KBDKeyEvent);
		message->keyCode = msg->keyCode;
		message->keyState = msg->keyState;

		if( targetApplication.empty() == false ) {
			sys::Bus::SendUnicast(message, targetApplication, this);
		}
		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMFocusApplication) ) {
		sevm::EVMFocusApplication* msg = reinterpret_cast<sevm::EVMFocusApplication*>( msgl );
		if( msg->sender == "ApplicationManager" ) {
			targetApplication = msg->getApplication();
			handled = true;
		}
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes EventManager::InitHandler() {

	//initialize keyboard worker
	EventWorker = new WorkerEvent( this );

	//create queues for worker
	sys::WorkerQueueInfo qTimer = {"qTimer", sizeof(bool), 10 };
	sys::WorkerQueueInfo qIrq = {"qIrq", sizeof(uint8_t), 10 };
	std::list<sys::WorkerQueueInfo> list;

	list.push_back(qTimer);
	list.push_back(qIrq);

	EventWorker->init( list );
	EventWorker->run();

	std::vector<xQueueHandle> set = EventWorker->getQueues();

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::DeinitHandler() {

	EventWorker->deinit();
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes EventManager::SleepHandler() {
	return sys::ReturnCodes::Success;
}

bool EventManager::messageSetApplication( sys::Service* sender, const std::string& applicationName ) {

	auto msg = std::make_shared<sevm::EVMFocusApplication>( applicationName );
	auto ret =  sys::Bus::SendUnicast(msg, "EventManager", sender,500  );
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}



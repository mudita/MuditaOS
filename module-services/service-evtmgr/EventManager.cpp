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

#include "vfs.hpp"


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

		auto message = std::make_shared<sevm::KbdMessage>(MessageType::KBDKeyEvent);
		message->keyCode = msg->keyCode;
		message->keyState = msg->keyState;
		message->keyPressTime = msg->keyPressTime;
		message->keyRelaseTime = msg->keyRelaseTime;

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
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMBatteryLevel) &&
		msgl->sender == this->GetName()) {
		sevm::BatteryLevelMessage* msg = reinterpret_cast<sevm::BatteryLevelMessage*>(msgl);

		auto message = std::make_shared<sevm::BatteryLevelMessage>(MessageType::EVMBatteryLevel);
		message->levelPercents = msg->levelPercents;
		message->fullyCharged = msg->fullyCharged;

		if( targetApplication.empty() == false ) {
			sys::Bus::SendUnicast(message, targetApplication, this);
		}
		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMChargerPlugged) &&
		msgl->sender == this->GetName()) {
		sevm::BatteryPlugMessage* msg = reinterpret_cast<sevm::BatteryPlugMessage*>(msgl);

		auto message = std::make_shared<sevm::BatteryPlugMessage>(MessageType::EVMChargerPlugged);
		message->plugged = msg->plugged;

		if( targetApplication.empty() == false ) {
			sys::Bus::SendUnicast(message, targetApplication, this);
		}
		handled = true;
	}

	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMMinuteUpdated) &&
			msgl->sender == this->GetName() ){

		sevm::RtcMinuteAlarmMessage* msg = reinterpret_cast<sevm::RtcMinuteAlarmMessage*>(msgl);

		auto message = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
		message->timestamp = msg->timestamp;

		if( targetApplication.empty() == false ){
			sys::Bus::SendUnicast(message, targetApplication, this);
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
	//keyboard irq queue
	sys::WorkerQueueInfo qIrq = {"qIrq", sizeof(uint8_t), 10 };
	//battery manager queue
	sys::WorkerQueueInfo qBattery = {"qBattery", sizeof(uint8_t), 10 };
	//RTC irq queue
	sys::WorkerQueueInfo qRTC = {"qRTC", sizeof(uint8_t), 20 };

	std::list<sys::WorkerQueueInfo> list;

	list.push_back(qIrq);
	list.push_back(qBattery);
	list.push_back(qRTC);

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
//	auto ret =  sys::Bus::SendUnicast(msg, "EventManager", sender, 200 );
//	return (ret.first == sys::ReturnCodes::Success )?true:false;
	sys::Bus::SendUnicast(msg, "EventManager", sender );
	return true;
}



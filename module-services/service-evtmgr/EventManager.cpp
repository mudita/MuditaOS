/*
 * EventManager.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "EventManager.hpp"

#include "log/log.hpp"

#include "bsp/keyboard/keyboard.hpp"
#include "WorkerEvent.hpp"
#include "messages/EVMessages.hpp"

#include "vfs.hpp"

#include "bsp/battery-charger/battery_charger.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"

EventManager::EventManager(const std::string& name)
		: sys::Service(name)
{
	LOG_INFO("[EventManager] Initializing");

	alarmTimestamp = 0;
	alarmID = 0;
	busChannels.push_back(sys::BusChannels::ServiceDatabaseAlarmNotifications);
    // each 30 sec pollTimer execute check - right now polling for battery only
    pollTimerID = CreateTimer(30000, true);
    ReloadTimer(pollTimerID);
}

EventManager::~EventManager(){

	LOG_INFO("[EventManager] Cleaning resources");
	if( EventWorker != nullptr) {
		EventWorker->deinit();
	}
}

// Invoked upon receiving data message
sys::Message_t EventManager::DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) {

	bool handled = false;

	if(msgl->messageType == static_cast<uint32_t>(MessageType::DBAlarmUpdateNotification))
	{

		alarmDBEmpty = false;
		alarmIsValid = false;
	}
	if(msgl->messageType == static_cast<uint32_t>(MessageType::KBDKeyEvent) &&
		msgl->sender == this->GetName()) {

		sevm::KbdMessage* msg = reinterpret_cast<sevm::KbdMessage*>(msgl);

		auto message = std::make_shared<sevm::KbdMessage>(MessageType::KBDKeyEvent);
        message->key = msg->key;

        if (suspended && (message->key.state == RawKey::State::Pressed))
        {
            suspended = false;
            sys::SystemManager::ResumeSystem(this);
        }

                //send key to focused application
		if( targetApplication.empty() == false ) {
			sys::Bus::SendUnicast(message, targetApplication, this);
		}
		//notify application manager to prevent screen locking
		sapm::ApplicationManager::messagePreventBlocking(this);
		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMFocusApplication) ) {
		sevm::EVMFocusApplication* msg = reinterpret_cast<sevm::EVMFocusApplication*>( msgl );
		if( msg->sender == "ApplicationManager" ) {
			targetApplication = msg->getApplication();
			handled = true;
			LOG_INFO("Switching focus to %s", targetApplication.c_str());
		}
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMBatteryLevel) &&
		msgl->sender == this->GetName()) {
		sevm::BatteryLevelMessage* msg = reinterpret_cast<sevm::BatteryLevelMessage*>(msgl);

		if( suspended ) {
			suspended = false;
			sys::SystemManager::ResumeSystem(this);
		}

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

		if( suspended ) {
			suspended = false;
			sys::SystemManager::ResumeSystem(this);
		}

		auto message = std::make_shared<sevm::BatteryPlugMessage>(MessageType::EVMChargerPlugged);
		message->plugged = msg->plugged;

		if( targetApplication.empty() == false ) {
			sys::Bus::SendUnicast(message, targetApplication, this);
		}
		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMMinuteUpdated) &&
			msgl->sender == this->GetName() ){

		//resume system first
		if( suspended ) {
			//suspended = false;
			//sys::SystemManager::ResumeSystem(this);
		}

		//HandleAlarmTrigger(msgl);

		handled = true;
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

void EventManager::handleBatPolling()
{
    bool charging = false;
    uint8_t newLevel = 0;
    static uint8_t level = 0;
    bsp::battery_getBatteryLevel(newLevel);
    bsp::battery_getChargeStatus(charging);
    if (level != newLevel && !charging)
    {
        LOG_DEBUG("Battery polled state change: %s, battery level: %d", charging ? "charging" : "discharging", level);
        auto message = std::make_shared<sevm::BatteryLevelMessage>(MessageType::EVMBatteryLevel);
        message->levelPercents = newLevel;
        sys::Bus::SendUnicast(message, "EventManager", this);
    }
    level = newLevel;
}

void EventManager::TickHandler(uint32_t id)
{
    if (id == pollTimerID)
    {
        handleBatPolling();
    }
}

// Invoked during initialization
sys::ReturnCodes EventManager::InitHandler() {

	//initialize keyboard worker
	EventWorker = std::make_unique<WorkerEvent>(this);

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
	EventWorker.reset();
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::SwitchPowerModeHandler(const sys::ServicePowerMode mode) {
    LOG_FATAL("[ServiceEvtMgr] PowerModeHandler: %d", static_cast<uint32_t>(mode));

    suspended = true;

    switch (mode){
        case sys::ServicePowerMode ::Active:
        	suspended = false;
            break;
        case sys::ServicePowerMode ::SuspendToRAM:
        case sys::ServicePowerMode ::SuspendToNVM:
            break;
    }

    return sys::ReturnCodes::Success;
}

bool EventManager::messageSetApplication(sys::Service *sender, const std::string &applicationName)
{

    auto msg = std::make_shared<sevm::EVMFocusApplication>(applicationName);
    return sys::Bus::SendUnicast(msg, "EventManager", sender);
}

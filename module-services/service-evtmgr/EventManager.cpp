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
#include "service-db/messages/DBNotificationMessage.hpp"

#include "bsp/harness/bsp_harness.hpp"
#include "harness/Parser.hpp"
#include "harness/events/AtResponse.hpp"
#include "harness/events/FocusApp.hpp"
#include <service-cellular/messages/CellularMessage.hpp>
#include <service-evtmgr/Constants.hpp>
#include <cassert>

EventManager::EventManager(const std::string &name) : sys::Service(name)
{
    LOG_INFO("[%s] Initializing", name.c_str());
    alarmTimestamp = 0;
    alarmID        = 0;
    busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
}

EventManager::~EventManager()
{

    LOG_INFO("[%s] Cleaning resources", GetName().c_str());
    if (EventWorker != nullptr) {
        EventWorker->deinit();
    }
}

// Invoked upon receiving data message
sys::Message_t EventManager::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    bool handled = false;

    if (auto msg = dynamic_cast<cellular::RawCommandResp *>(resp)) {
        bsp::harness::emit(harness::AtResponse(msg->response).encode());
        handled = true;
    }
    if (msgl->messageType == MessageType::DBServiceNotification) {
        auto *msg = dynamic_cast<DBNotificationMessage *>(msgl);
        if (msg != nullptr) {
            if ((msg->baseType == DB::BaseType::AlarmDB) && ((msg->notificationType == DB::NotificationType::Updated) ||
                                                             (msg->notificationType == DB::NotificationType::Added))) {
                alarmDBEmpty = false;
                alarmIsValid = false;
                handled      = true;
            }
        }
    }
    else if (msgl->messageType == MessageType::EVM_GPIO) {
        LOG_DEBUG("EVM_GPIO msg");
    }
    else if (msgl->messageType == MessageType::KBDKeyEvent && msgl->sender == this->GetName()) {

        auto *msg = dynamic_cast<sevm::KbdMessage *>(msgl);
        assert(msg);
        auto message = std::make_shared<sevm::KbdMessage>();
        message->key = msg->key;

        if (suspended && (message->key.state == RawKey::State::Pressed)) {
            suspended = false;
            sys::SystemManager::ResumeSystem(this);
        }
        if (message->key.state == RawKey::State::Pressed && message->key.key_code == bsp::KeyCodes::FnRight) {
            // and state == ShutDown
            sys::Bus::SendUnicast(message, service::name::system_manager, this);
        }

        // send key to focused application
        if (targetApplication.empty() == false) {
            sys::Bus::SendUnicast(message, targetApplication, this);
        }
        // notify application manager to prevent screen locking
        sapm::ApplicationManager::messagePreventBlocking(this);
        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMFocusApplication) {
        auto *msg = reinterpret_cast<sevm::EVMFocusApplication *>(msgl);
        if (msg->sender == "ApplicationManager") {
            targetApplication = msg->getApplication();
            handled           = true;
            LOG_INFO("Switching focus to %s", targetApplication.c_str());
            bsp::harness::emit(harness::FocusApp(targetApplication).encode());
        }
    }
    else if (msgl->messageType == MessageType::EVMBatteryLevel && msgl->sender == this->GetName()) {
        sevm::BatteryLevelMessage *msg = reinterpret_cast<sevm::BatteryLevelMessage *>(msgl);

        if (suspended) {
            suspended = false;
            sys::SystemManager::ResumeSystem(this);
        }

        auto message           = std::make_shared<sevm::BatteryLevelMessage>();
        message->levelPercents = msg->levelPercents;
        message->fullyCharged  = msg->fullyCharged;

        if (targetApplication.empty() == false) {
            sys::Bus::SendUnicast(message, targetApplication, this);
        }

        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMChargerPlugged && msgl->sender == this->GetName()) {
        sevm::BatteryPlugMessage *msg = reinterpret_cast<sevm::BatteryPlugMessage *>(msgl);

        if (suspended) {
            suspended = false;
            sys::SystemManager::ResumeSystem(this);
        }

        auto message     = std::make_shared<sevm::BatteryPlugMessage>();
        message->plugged = msg->plugged;

        if (!message->plugged) {
            sys::Bus::SendUnicast(message, service::name::system_manager, this);
        }

        if (targetApplication.empty() == false) {
            sys::Bus::SendUnicast(message, targetApplication, this);
        }
        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMMinuteUpdated && msgl->sender == this->GetName()) {

        // resume system first
        if (suspended) {
            // suspended = false;
            // sys::SystemManager::ResumeSystem(this);
        }

        // HandleAlarmTrigger(msgl);

        handled = true;
    }
    else if (!targetApplication.empty() && dynamic_cast<sevm::SIMMessage *>(msgl) != nullptr) {
        sys::Bus::SendUnicast(std::make_shared<sevm::SIMMessage>(), targetApplication, this);
    }

    if (handled)
        return std::make_shared<sys::ResponseMessage>();
    else
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes EventManager::InitHandler()
{

    // initialize keyboard worker
    EventWorker = std::make_unique<WorkerEvent>(this);

    // create queues for worker
    // keyboard irq queue
    sys::WorkerQueueInfo qIrq = {"qIrq", sizeof(uint8_t), 10};
    // battery manager queue
    sys::WorkerQueueInfo qBattery = {"qBattery", sizeof(uint8_t), 10};
    // RTC irq queue
    sys::WorkerQueueInfo qRTC = {"qRTC", sizeof(uint8_t), 20};
    // test harness queue
    sys::WorkerQueueInfo qHarness = {"qHarness", sizeof(uint8_t), 3};
    // sim tray queue
    sys::WorkerQueueInfo qSIM = {"qSIM", sizeof(uint8_t), 5};

    std::list<sys::WorkerQueueInfo> list;

    list.push_back(qIrq);
    list.push_back(qBattery);
    list.push_back(qRTC);
    list.push_back(qHarness);
    list.push_back(qSIM);

    EventWorker->init(list);
    EventWorker->run();

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::DeinitHandler()
{

    EventWorker->deinit();
    EventWorker.reset();
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceEvtMgr] PowerModeHandler: %s", c_str(mode));

    suspended = true;

    switch (mode) {
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
    return sys::Bus::SendUnicast(msg, service::name::evt_manager, sender);
}

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * EventManager.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include "EventManager.hpp"

#include <service-cellular/messages/CellularMessage.hpp> // for CellularTimeNotificationMessage, RawCommandResp
#include <service-evtmgr/Constants.hpp>                  // for evt_manager
#include <cassert>                                       // for assert
#include <list>                                          // for list
#include <tuple>                                         // for tie, tuple
#include <vector>                                        // for vector

#include "log/log.hpp"             // for LOG_INFO, LOG_DEBUG, LOG_FATAL
#include "WorkerEvent.hpp"         // for WorkerEvent
#include "messages/EVMessages.hpp" // for TorchStateResultMessage, EVMFocusApplication, StatusStateMessage, EVMBoardResponseMessage, SIMMessage, TorchStateMessage
#include "service-appmgr/Controller.hpp"                 // for Controller
#include "service-db/messages/DBNotificationMessage.hpp" // for NotificationMessage
#include "AudioServiceAPI.hpp"                           // for SendEvent
#include "bsp/harness/bsp_harness.hpp"                   // for emit
#include "harness/events/AtResponse.hpp"                 // for AtResponse
#include "harness/events/FocusApp.hpp"                   // for FocusApp
#include "bsp/magnetometer/magnetometer.hpp"             // for GetBoard
#include "bsp/common.hpp"                                // for c_str
#include "bsp/rtc/rtc.hpp"                               // for rtc_SetDateTime
#include "BaseInterface.hpp"                             // for Interface, Interface::Name, Interface::Name::Alarms
#include "MessageType.hpp" // for MessageType, MessageType::EVMModemStatus, MessageType::CellularTimeUpdated, MessageType::DBServiceNotification, MessageType::EVMBatteryLevel, MessageType::EVMChargerPlugged, MessageType::EVMFocusApplication, MessageType::EVMGetBoard, MessageType::EVMMinuteUpdated, MessageType::EVMRingIndicator, MessageType::EVMTimeUpdated, MessageType::EVMTorchStateMessage, MessageType::EVM_GPIO, MessageType::KBDKeyEvent
#include "Service/Bus.hpp" // for Bus
#include "Service/Worker.hpp"              // for WorkerQueueInfo
#include "SystemManager/Constants.hpp"     // for system_manager
#include "SystemManager/SystemManager.hpp" // for SystemManager
#include "bsp/keyboard/key_codes.hpp"      // for KeyCodes, KeyCodes::FnRight, bsp
#include "bsp/torch/torch.hpp" // for State, Action, getColorTemp, getState, toggle, turn, Action::getState, Action::setState, Action::toggle
#include "common_data/RawKey.hpp"                      // for RawKey, RawKey::State, RawKey::State::Pressed
#include "service-audio/messages/AudioMessage.hpp"     // for AudioEventRequest
#include "service-evtmgr/messages/BatteryMessages.hpp" // for BatteryLevelMessage, BatteryPlugMessage
#include "service-evtmgr/messages/KbdMessage.hpp"      // for KbdMessage
#include "module-utils/time/time_conversion.hpp"       // for Time Zone handling

EventManager::EventManager(const std::string &name)
    : sys::Service(name), vibratorTimerPeriodic(std::make_unique<sys::Timer>(
                              "VibratorTimerPeriodic", this, sys::Timer::timeout_infinite, sys::Timer::Type::Periodic)),
      vibratorTimerOneshot(std::make_unique<sys::Timer>(
          "VibratorTimerOneshot", this, sys::Timer::timeout_infinite, sys::Timer::Type::SingleShot))
{
    LOG_INFO("[%s] Initializing", name.c_str());
    alarmTimestamp = 0;
    alarmID        = 0;
    busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
}

EventManager::~EventManager()
{
    if (EventWorker != nullptr) {
        EventWorker->close();
    }
}

// those static functions and variables will be replaced by Settings API
static std::string tzSet;
static void setSettingsTimeZone(const std::string &timeZone)
{
    tzSet = timeZone;
}
std::string getSettingsTimeZone()
{
    return tzSet;
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
        auto *msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->interface == db::Interface::Name::Alarms) {
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
        app::manager::Controller::preventBlockingDevice(this);
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
    else if (auto msg = dynamic_cast<AudioEventRequest *>(msgl); msg /*&& msgl->sender == this->GetName()*/) {
        AudioServiceAPI::SendEvent(this, msg->getEvent());
        handled = true;
    }
    else if (!targetApplication.empty() && dynamic_cast<sevm::SIMMessage *>(msgl) != nullptr) {
        sys::Bus::SendUnicast(std::make_shared<sevm::SIMMessage>(), targetApplication, this);
    }
    else if (msgl->messageType == MessageType::EVMGetBoard) {
        using namespace bsp;

        auto msg   = std::make_shared<sevm::EVMBoardResponseMessage>(true);
        auto board = magnetometer::GetBoard();
        msg->board = board;
        LOG_INFO("Board discovered: %s", c_str(board));

        return msg;
    }
    else if (msgl->messageType == MessageType::EVMModemStatus) {
        auto msg = dynamic_cast<sevm::StatusStateMessage *>(msgl);
        if (msg != nullptr) {
            auto message   = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMModemStatus);
            message->state = msg->state;
            sys::Bus::SendUnicast(message, "ServiceCellular", this);
        }
        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMVibratorStateMessage) {
        using namespace bsp;

        auto msgVibraState = static_cast<sevm::VibratorStateMessage *>(msgl);

        // invalidate any pending timer regardless of its scheduled action, because there will be new action
        vibratorTimerPeriodic->stop();
        vibratorTimerOneshot->stop();

        vibrator::set(msgVibraState->state);
    }
    else if (msgl->messageType == MessageType::EVMVibratorPulseMessage) {
        using namespace bsp;

        auto msgVibraPulse = static_cast<sevm::VibratorPulseMessage *>(msgl);

        vibratorTimerPeriodic->stop();
        vibratorTimerOneshot->stop();

        vibrator::set(vibrator::State::On);

        if (msgVibraPulse->vibration.period > std::chrono::milliseconds::zero()) {
            // periodic vibration pulses
            vibratorTimerPeriodic->setInterval(msgVibraPulse->vibration.period.count());
            vibratorTimerPeriodic->connect([&](sys::Timer &timer) {
                vibrator::set(vibrator::State::On);
                vibratorTimerOneshot->reload();
            });
            vibratorTimerPeriodic->start();
        }
        else {
            // single vibration pulse
        }
        vibratorTimerOneshot->setInterval(msgVibraPulse->vibration.pulse.count());
        vibratorTimerOneshot->connect([&](sys::Timer &) { vibrator::set(vibrator::State::Off); });
        vibratorTimerOneshot->start();
    }

    else if (msgl->messageType == MessageType::EVMTorchStateMessage) {
        auto msg = dynamic_cast<sevm::TorchStateMessage *>(msgl);
        if (msg != nullptr) {
            auto message = std::make_shared<sevm::TorchStateResultMessage>(msg->action);

            switch (msg->action) {
            case bsp::torch::Action::getState:
                std::tie(message->success, message->state) = bsp::torch::getState();
                message->colourTemp                        = bsp::torch::getColorTemp();
                break;
            case bsp::torch::Action::setState:
                message->success = bsp::torch::turn(msg->state, msg->colourTemp);
                break;
            case bsp::torch::Action::toggle:
                message->success = bsp::torch::toggle();
                break;
            }
            return message;
        }
    }
    else if (msgl->messageType == MessageType::CellularTimeUpdated) {
        auto msg = dynamic_cast<CellularTimeNotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (auto time = msg->getTime(); time) {
                LOG_INFO("RTC set by network time.");
                bsp::rtc_SetDateTime(&time.value());
            }
            if (auto timeZoneOffset = msg->getTimeZoneOffset(); timeZoneOffset) {
                setSettingsTimeZone(msg->getTimeZoneString().value());
                utils::time::Time::setTimeZoneOffset(msg->getTimeZoneOffset().value());
            }
            auto notification = std::make_shared<sys::DataMessage>(MessageType::EVMTimeUpdated);
            sys::Bus::SendMulticast(notification, sys::BusChannels::ServiceEvtmgrNotifications, this);
        }
    }
    else if (msgl->messageType == MessageType::EVMRingIndicator) {
        sys::SystemManager::ResumeSystem(this);
    }

    if (handled)
        return std::make_shared<sys::ResponseMessage>();
    else
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes EventManager::InitHandler()
{

    // initialize bsp worker
    EventWorker = std::make_unique<WorkerEvent>(this);

    // create queues for worker
    // keyboard irq queue
    sys::WorkerQueueInfo qIrq = {"qIrq", sizeof(uint8_t), 10};
    // headset irq queue
    sys::WorkerQueueInfo qHeadset = {"qHeadset", sizeof(uint8_t), 10};
    // battery manager queue
    sys::WorkerQueueInfo qBattery = {"qBattery", sizeof(uint8_t), 10};
    // RTC irq queue
    sys::WorkerQueueInfo qRTC = {"qRTC", sizeof(uint8_t), 20};
    // test harness queue
    sys::WorkerQueueInfo qHarness = {"qHarness", sizeof(uint8_t), 3};
    // sim tray queue
    sys::WorkerQueueInfo qSIM = {"qSIM", sizeof(uint8_t), 5};
    // magnetometer queue
    sys::WorkerQueueInfo qMagnetometer = {"qMagnetometer", sizeof(uint8_t), 5};
    // torch driver queue
    sys::WorkerQueueInfo qTorch = {"qTorch", sizeof(uint8_t), 5};

    std::list<sys::WorkerQueueInfo> list;

    list.push_back(qIrq);
    list.push_back(qHeadset);
    list.push_back(qBattery);
    list.push_back(qRTC);
    list.push_back(qHarness);
    list.push_back(qSIM);
    list.push_back(qMagnetometer);
    list.push_back(qTorch);

    EventWorker->init(list);
    EventWorker->run();

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::DeinitHandler()
{
    EventWorker->close();
    EventWorker.reset();
    EventWorker = nullptr;

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

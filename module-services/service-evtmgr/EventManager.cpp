// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/BatteryMessages.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/EventManager.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/WorkerEvent.hpp"
#include "service-evtmgr/ScreenLightControl.hpp"

#include <BaseInterface.hpp>
#include <MessageType.hpp>
#include <Service/Bus.hpp>
#include <Service/Worker.hpp>
#include <SystemManager/Constants.hpp>
#include <SystemManager/SystemManager.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/torch/torch.hpp>
#include <common_data/RawKey.hpp>
#include <log/log.hpp>
#include <module-utils/time/time_conversion.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <cassert>
#include <list>
#include <tuple>
#include <vector>
#include <module-apps/messages/AppMessage.hpp>

EventManager::EventManager(const std::string &name) : sys::Service(name)
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
sys::MessagePointer EventManager::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    bool handled = false;

    if (auto msg = dynamic_cast<cellular::RawCommandResp *>(resp)) {

        auto event   = std::make_unique<sdesktop::developerMode::ATResponseEvent>(msg->response);
        auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
        sys::Bus::SendUnicast(message, service::name::service_desktop, this);

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
    else if (msgl->messageType == MessageType::KBDKeyEvent &&
             (msgl->sender == this->GetName() || msgl->sender == service::name::service_desktop)) {

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
        if (!targetApplication.empty()) {
            sys::Bus::SendUnicast(message, targetApplication, this);
        }
        // notify application manager to prevent screen locking
        app::manager::Controller::preventBlockingDevice(this);
        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMFocusApplication) {
        auto *msg = static_cast<sevm::EVMFocusApplication *>(msgl);
        if (msg->sender == "ApplicationManager") {
            targetApplication = msg->getApplication();
            handled           = true;
            LOG_INFO("Switching focus to %s", targetApplication.c_str());
        }
    }
    else if (msgl->messageType == MessageType::EVMBatteryLevel && msgl->sender == this->GetName()) {
        auto *msg = static_cast<sevm::BatteryLevelMessage *>(msgl);

        if (suspended) {
            suspended = false;
            sys::SystemManager::ResumeSystem(this);
        }

        auto message           = std::make_shared<sevm::BatteryLevelMessage>();
        message->levelPercents = msg->levelPercents;
        message->fullyCharged  = msg->fullyCharged;

        if (!targetApplication.empty()) {
            sys::Bus::SendUnicast(message, targetApplication, this);
        }

        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMChargerPlugged && msgl->sender == this->GetName()) {
        auto *msg = static_cast<sevm::BatteryPlugMessage *>(msgl);

        if (suspended) {
            suspended = false;
            sys::SystemManager::ResumeSystem(this);
        }

        auto message     = std::make_shared<sevm::BatteryPlugMessage>();
        message->plugged = msg->plugged;

        if (!message->plugged) {
            sys::Bus::SendUnicast(message, service::name::system_manager, this);
        }

        if (!targetApplication.empty()) {
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

    if (handled) {
        return std::make_shared<sys::ResponseMessage>();
    }
    else {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }
}

// Invoked during initialization
sys::ReturnCodes EventManager::InitHandler()
{

    connect(sdesktop::developerMode::DeveloperModeRequest(), [&](sys::Message *msg) {
        using namespace sdesktop::developerMode;
        auto req = static_cast<DeveloperModeRequest *>(msg);
        if (typeid(*req->event.get()) == typeid(AppFocusChangeEvent)) {
            auto event   = std::make_unique<AppFocusChangeEvent>(targetApplication);
            auto message = std::make_shared<DeveloperModeRequest>(std::move(event));
            sys::Bus::SendUnicast(message, service::name::service_desktop, this);
        }

        return std::make_shared<sys::ResponseMessage>();
    });

    connect(app::AppInputEventMessage(gui::InputEvent(RawKey())), [&](sys::Message *msgl) {
        auto msg = static_cast<app::AppInputEventMessage *>(msgl);
        assert(msg);

        auto message = std::make_shared<app::AppInputEventMessage>(msg->getEvent());
        if (!targetApplication.empty()) {
            sys::Bus::SendUnicast(std::move(message), targetApplication, this);
        }

        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::KeypadBacklightMessage(), [&](sys::Message *msgl) {
        auto request      = static_cast<sevm::KeypadBacklightMessage *>(msgl);
        auto response     = std::make_shared<sevm::KeypadBacklightResponseMessage>();
        response->success = processKeypadBacklightRequest(request->action);
        return response;
    });

    connect(sevm::ScreenLightControlMessage(), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::ScreenLightControlMessage *>(msgl);
        sevm::screen_light_control::processRequest(request->action, request->parameters);
        return std::make_shared<sys::ResponseMessage>();
    });

    // initialize keyboard worker
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
    // sim tray queue
    sys::WorkerQueueInfo qSIM = {"qSIM", sizeof(uint8_t), 5};
    // magnetometer queue
    sys::WorkerQueueInfo qMagnetometer = {"qMagnetometer", sizeof(uint8_t), 5};
    // torch driver queue
    sys::WorkerQueueInfo qTorch = {"qTorch", sizeof(uint8_t), 5};
    // light sensor queue
    sys::WorkerQueueInfo qLightSensor = {"qLightSensor", sizeof(uint8_t), 5};

    std::list<sys::WorkerQueueInfo> list;

    list.push_back(qIrq);
    list.push_back(qHeadset);
    list.push_back(qBattery);
    list.push_back(qRTC);
    list.push_back(qSIM);
    list.push_back(qMagnetometer);
    list.push_back(qTorch);
    list.push_back(qLightSensor);

    EventWorker->init(list);
    EventWorker->run();
    sevm::screen_light_control::init(this);

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::DeinitHandler()
{
    sevm::screen_light_control::deinit();
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

bool EventManager::processKeypadBacklightRequest(bsp::keypad_backlight::Action act)
{
    bool response = false;
    switch (act) {
    case bsp::keypad_backlight::Action::turnOn:
        response = bsp::keypad_backlight::turnOnAll();
        break;
    case bsp::keypad_backlight::Action::turnOff:
        response = bsp::keypad_backlight::shutdown();
        break;
    case bsp::keypad_backlight::Action::checkState:
        response = bsp::keypad_backlight::checkState();
        break;
    }
    return response;
}
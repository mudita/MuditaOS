// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/BatteryMessages.hpp"
#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/ScreenLightControlMessage.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/EventManager.hpp"
#include "service-evtmgr/WorkerEvent.hpp"

#include "battery-level-check/BatteryLevelCheck.hpp"
#include "screen-light-control/ScreenLightControl.hpp"

#include <BaseInterface.hpp>
#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <Timers/TimerFactory.hpp>
#include <SystemManager/Constants.hpp>
#include <SystemManager/SystemManager.hpp>
#include <bsp/common.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/battery-charger/battery_charger.hpp>
#include <common_data/RawKey.hpp>
#include <log/log.hpp>
#include <log/Logger.hpp>
#include <module-utils/time/time_conversion.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <list>
#include <tuple>
#include <vector>
#include <module-apps/messages/AppMessage.hpp>
#include <SystemManager/messages/CpuFrequencyMessage.hpp>
#include <common_data/EventStore.hpp>
#include <SystemManager/messages/PhoneModeRequest.hpp>
#include <vibra/Vibra.hpp>

namespace
{
    constexpr auto loggerDelayMs   = 1000 * 60 * 5;
    constexpr auto loggerTimerName = "Logger";
} // namespace

EventManager::EventManager(const std::string &name)
    : sys::Service(name, "", stackDepth),
      settings(std::make_shared<settings::Settings>(this)), loggerTimer{sys::TimerFactory::createPeriodicTimer(
                                                                this,
                                                                loggerTimerName,
                                                                std::chrono::milliseconds{loggerDelayMs},
                                                                [this](sys::Timer & /*timer*/) { dumpLogsToFile(); })},
      screenLightControl(std::make_unique<screen_light_control::ScreenLightControl>(settings, this)),
      Vibra(std::make_unique<vibra_handle::Vibra>(this))
{
    LOG_INFO("[%s] Initializing", name.c_str());
    alarmTimestamp = 0;
    alarmID        = 0;
    bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    loggerTimer.start();
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

        if (message->key.state == RawKey::State::Pressed) {
            const auto code = message->key.key_code;
            if (code == bsp::KeyCodes::FnRight) {
                bus.sendUnicast(message, service::name::system_manager);
            }
            else if (code == bsp::KeyCodes::SSwitchUp || code == bsp::KeyCodes::SSwitchMid ||
                     code == bsp::KeyCodes::SSwitchDown) {
                const auto mode = sys::SystemManager::translateSliderState(message->key);
                bus.sendUnicast(std::make_shared<sys::PhoneModeRequest>(mode), service::name::system_manager);
            }
        }

        // send key to focused application
        if (!targetApplication.empty()) {
            bus.sendUnicast(message, targetApplication);
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
    else if (msgl->messageType == MessageType::EVMMinuteUpdated && msgl->sender == this->GetName()) {
        auto msg = static_cast<sevm::RtcMinuteAlarmMessage *>(msgl);
        handleMinuteUpdate(msg->timestamp);
        handled = true;
    }
    else if (auto msg = dynamic_cast<AudioEventRequest *>(msgl); msg) {
        AudioServiceAPI::SendEvent(this, msg->getEvent());
        handled = true;
    }
    else if (!targetApplication.empty() && dynamic_cast<sevm::SIMMessage *>(msgl) != nullptr) {
        bus.sendUnicast(std::make_shared<sevm::SIMMessage>(), targetApplication);
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
            bus.sendUnicast(message, "ServiceCellular");
        }
        handled = true;
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
            bus.sendMulticast(notification, sys::BusChannel::ServiceEvtmgrNotifications);
        }
    }
    else if (msgl->messageType == MessageType::EVMRingIndicator) {
        auto msg = std::make_shared<CellularUrcIncomingNotification>();
        bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
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
            bus.sendUnicast(message, service::name::service_desktop);
        }

        return std::make_shared<sys::ResponseMessage>();
    });

    connect(app::AppInputEventMessage(gui::InputEvent(RawKey())), [&](sys::Message *msgl) {
        auto msg = static_cast<app::AppInputEventMessage *>(msgl);
        assert(msg);

        auto message = std::make_shared<app::AppInputEventMessage>(msg->getEvent());
        if (!targetApplication.empty()) {
            bus.sendUnicast(std::move(message), targetApplication);
        }

        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::KeypadBacklightMessage(bsp::keypad_backlight::Action::turnOff), [&](sys::Message *msgl) {
        auto request      = static_cast<sevm::KeypadBacklightMessage *>(msgl);
        auto response     = std::make_shared<sevm::KeypadBacklightResponseMessage>();
        response->success = processKeypadBacklightRequest(request->action);
        return response;
    });

    connect(sevm::BatterySetCriticalLevel(0), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::BatterySetCriticalLevel *>(msgl);
        battery_level_check::setBatteryCriticalLevel(request->criticalLevel);
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::ScreenLightControlMessage(), [&](sys::Message *msgl) {
        auto *m = dynamic_cast<sevm::ScreenLightControlMessage *>(msgl);
        screenLightControl->processRequest(m->action, m->parameters);
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::ScreenLightControlRequestParameters(), [&](sys::Message *msgl) {
        screen_light_control::Parameters params = {screenLightControl->getBrightnessValue()};
        auto msg                                = std::make_shared<sevm::ScreenLightControlParametersResponse>(
            screenLightControl->getLightState(), screenLightControl->getAutoModeState(), params);
        return msg;
    });
    connect(sevm::RtcUpdateTimeMessage(0), [&](sys::Message *msgl) {
        auto msg = static_cast<sevm::RtcUpdateTimeMessage *>(msgl);
        bsp::rtc_SetDateTimeFromTimestamp(msg->getTime());
        bsp::rtc_SetMinuteAlarm(msg->getTime());
        handleMinuteUpdate(msg->getTime());
        return sys::msgHandled();
    });

    connect(sevm::BatteryStatusChangeMessage(), [&](sys::Message *msgl) {
        if (msgl->sender == this->GetName()) {
            LOG_INFO("Battery level: %d , charging: %d",
                     Store::Battery::get().level,
                     Store::Battery::get().state == Store::Battery::State::Charging);

            if (Store::Battery::get().state == Store::Battery::State::Discharging) {
                bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), service::name::system_manager);
            }

            if (!targetApplication.empty()) {
                bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), targetApplication);
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::VibraMessage(bsp::vibrator::Action::stop), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::VibraMessage *>(msgl);
        processVibraRequest(request->action, request->repetitionTime);
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::ToggleTorchOnOffMessage(), [&](sys::Message *) {
        toggleTorchOnOff();
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sevm::ToggleTorchColorMessage(), [&](sys::Message *) {
        toggleTorchColor();
        return std::make_shared<sys::ResponseMessage>();
    });

    // initialize keyboard worker
    EventWorker = std::make_unique<WorkerEvent>(this);

    using namespace std::string_literals;
    std::list<sys::WorkerQueueInfo> list;
    list.emplace_back("qIrq"s, sizeof(uint8_t), 10);
    list.emplace_back("qHeadset"s, sizeof(uint8_t), 10);
    list.emplace_back("qBattery"s, sizeof(uint8_t), 10);
    list.emplace_back("qRTC"s, sizeof(uint8_t), 20);
    list.emplace_back("qSIM"s, sizeof(uint8_t), 5);
    list.emplace_back("qMagnetometer"s, sizeof(uint8_t), 5);
    list.emplace_back("qTorch"s, sizeof(uint8_t), 5);
    list.emplace_back("qLightSensor"s, sizeof(uint8_t), 5);
    list.emplace_back("qChargerDetect"s, sizeof(uint8_t), 5);

    EventWorker->init(list, settings);
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

void EventManager::ProcessCloseReason(sys::CloseReason closeReason)
{
    bsp::torch::turn(bsp::torch::State::off);
    sendCloseReadyMessage(this);
}

sys::ReturnCodes EventManager::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceEvtMgr] PowerModeHandler: %s", c_str(mode));

    switch (mode) {
    case sys::ServicePowerMode ::Active:
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
    return sender->bus.sendUnicast(msg, service::name::evt_manager);
}

void EventManager::dumpLogsToFile()
{
    const auto logPath = purefs::dir::getUserDiskPath() / LOG_FILE_NAME;
    const bool dumpLog = !(std::filesystem::exists(logPath) && std::filesystem::file_size(logPath) > MAX_LOG_FILE_SIZE);
    if (dumpLog) {
        const auto &logs = Log::Logger::get().getLogs();
        std::fstream logFile(logPath, std::fstream::out | std::fstream::app);
        logFile.write(logs.data(), logs.size());
    }
}

void EventManager::handleMinuteUpdate(time_t timestamp)
{
    if (!targetApplication.empty()) {
        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        bus.sendUnicast(message, targetApplication);
    }
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

bool EventManager::processVibraRequest(bsp::vibrator::Action act, std::chrono::milliseconds RepetitionTime)
{
    switch (act) {
    case bsp::vibrator::Action::pulse:
        Vibra->Pulse();
        break;
    case bsp::vibrator::Action::pulseRepeat:
        Vibra->PulseRepeat(RepetitionTime);
        break;
    case bsp::vibrator::Action::pulseRepeatInfinite:
        Vibra->PulseRepeat();
        break;
    case bsp::vibrator::Action::stop:
        Vibra->PulseRepeatStop();
        break;
    }
    return true;
}

void EventManager::toggleTorchOnOff()
{
    auto state    = bsp::torch::getState();
    auto newState = (state.second == bsp::torch::State::off) ? bsp::torch::State::on : bsp::torch::State::off;
    bsp::torch::turn(newState, bsp::torch::ColourTemperature::coldest);
}

void EventManager::toggleTorchColor()
{
    auto state = bsp::torch::getState();
    if (state.second == bsp::torch::State::on) {
        auto color    = bsp::torch::getColorTemp();
        auto newColor = (color == bsp::torch::ColourTemperature::coldest) ? bsp::torch::ColourTemperature::warmest
                                                                          : bsp::torch::ColourTemperature::coldest;
        bsp::torch::turn(bsp::torch::State::on, newColor);
    }
}

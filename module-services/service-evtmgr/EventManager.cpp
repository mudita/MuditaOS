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
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/battery-charger/battery_charger.hpp>
#include <bsp/keyboard/key_codes.hpp>
#include <log.hpp>
#include <Logger.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-time/service-time/TimeMessage.hpp>

#include <cassert>
#include <fstream>
#include <filesystem>
#include <list>
#include <ctime>
#include <apps-common/messages/AppMessage.hpp>
#include <EventStore.hpp>
#include <SystemManager/messages/PhoneModeRequest.hpp>
#include <vibra/Vibra.hpp>
#include <ticks.hpp>
#include <purefs/filesystem_paths.hpp>

namespace
{
    constexpr auto loggerDelayMs        = 1000 * 60 * 5;
    constexpr auto loggerTimerName      = "Logger";
    constexpr std::array sliderKeyCodes = {
        bsp::KeyCodes::SSwitchUp, bsp::KeyCodes::SSwitchMid, bsp::KeyCodes::SSwitchDown};

    [[nodiscard]] bool isSliderKeyCode(bsp::KeyCodes code)
    {
        return std::find(std::begin(sliderKeyCodes), std::end(sliderKeyCodes), code) != std::end(sliderKeyCodes);
    }

} // namespace

EventManager::EventManager(const std::string &name)
    : sys::Service(name, "", stackDepth),
      settings(std::make_shared<settings::Settings>()), loggerTimer{sys::TimerFactory::createPeriodicTimer(
                                                            this,
                                                            loggerTimerName,
                                                            std::chrono::milliseconds{loggerDelayMs},
                                                            [this](sys::Timer & /*timer*/) { dumpLogsToFile(); })},
      Vibra(std::make_unique<vibra_handle::Vibra>(this)), backlightHandler(settings, this)
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
    else if (msgl->messageType == MessageType::EVMGetBoard) {
        using namespace bsp;

        auto msg   = std::make_shared<sevm::EVMBoardResponseMessage>(true);
        auto board = magnetometer::GetBoard();
        msg->board = board;
        LOG_INFO("Board discovered: %s", c_str(board));

        return msg;
    }
    else if (msgl->messageType == MessageType::EVMModemStatus) {
        if (auto msg = dynamic_cast<sevm::StatusStateMessage *>(msgl)) {
            auto message   = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMModemStatus);
            message->state = msg->state;
            bus.sendUnicast(message, ServiceCellular::serviceName);
        }
        handled = true;
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
    settings->init(service::ServiceProxy(shared_from_this()));
    backlightHandler.init();

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

    connect(sevm::KbdMessage(), [&](sys::Message *msg) {
        handleKeyEvent(msg);
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
        response->success = backlightHandler.processKeypadRequest(request->action);
        return response;
    });

    connect(sevm::BatterySetCriticalLevel(0), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::BatterySetCriticalLevel *>(msgl);
        battery_level_check::setBatteryCriticalLevel(request->criticalLevel);
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightControlMessage), [&](sys::Message *msgl) {
        auto *m = dynamic_cast<sevm::ScreenLightControlMessage *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters());
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetAutoModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetAutoModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetManualModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetManualModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(sevm::ScreenLightControlRequestParameters(), [&](sys::Message *msgl) {
        screen_light_control::ManualModeParameters params = {backlightHandler.getScreenBrightnessValue()};
        auto msg = std::make_shared<sevm::ScreenLightControlParametersResponse>(
            backlightHandler.getScreenLightState(), backlightHandler.getScreenAutoModeState(), params);
        return msg;
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
        return sys::msgHandled();
    });

    connect(sevm::VibraMessage(bsp::vibrator::Action::stop), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::VibraMessage *>(msgl);
        processVibraRequest(request->action, request->repetitionTime);
        return sys::msgHandled();
    });

    connect(sevm::ToggleTorchOnOffMessage(), [&]([[maybe_unused]] sys::Message *msg) {
        toggleTorchOnOff();
        return sys::MessageNone{};
    });

    connect(sevm::ToggleTorchColorMessage(), [&]([[maybe_unused]] sys::Message *msg) {
        toggleTorchColor();
        return sys::MessageNone{};
    });

    connect(sevm::RequestPhoneModeForceUpdate(), [&]([[maybe_unused]] sys::Message *msg) {
        EventWorker->requestSliderPositionRead();
        return sys::MessageNone{};
    });

    connect(typeid(sevm::SIMTrayMessage), [&](sys::Message *) {
        bus.sendUnicast(std::make_shared<sevm::SIMTrayMessage>(), ServiceCellular::serviceName);
        return sys::MessageNone{};
    });

    connect(typeid(stm::message::UpdateRTCValueFromTmMessage), [&](sys::Message *msg) {
        auto message = static_cast<stm::message::UpdateRTCValueFromTmMessage *>(msg);
        processRTCFromTmRequest(message->getTime());
        return sys::MessageNone{};
    });

    connect(typeid(stm::message::UpdateRTCValueFromTimestampMessage), [&](sys::Message *msg) {
        auto message = static_cast<stm::message::UpdateRTCValueFromTimestampMessage *>(msg);
        processRTCFromTimestampRequest(message->getTime());
        return sys::MessageNone{};
    });

    connect(typeid(stm::message::UpdateTimeZoneMessage), [&](sys::Message *msg) {
        auto message = static_cast<stm::message::UpdateTimeZoneMessage *>(msg);
        processTimezoneRequest(message->getTimezone());
        return sys::MessageNone{};
    });

    connect(typeid(sevm::FlushLogsRequest), [&]([[maybe_unused]] sys::Message *msg) {
        if (auto ret = dumpLogsToFile(); ret >= 0) {
            return std::make_shared<sevm::FlushLogsResponse>(true, ret);
        }
        return std::make_shared<sevm::FlushLogsResponse>(false);
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
    list.emplace_back(WorkerEvent::MagnetometerNotifyQueue, sizeof(uint8_t), 1);
    list.emplace_back("qTorch"s, sizeof(uint8_t), 5);
    list.emplace_back("qLightSensor"s, sizeof(uint8_t), 5);
    list.emplace_back("qChargerDetect"s, sizeof(uint8_t), 5);

    EventWorker->init(list, settings);
    EventWorker->run();

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes EventManager::DeinitHandler()
{
    settings->deinit();

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

void EventManager::handleKeyEvent(sys::Message *msg)
{
    auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    auto message    = std::make_shared<sevm::KbdMessage>();
    message->key    = kbdMessage->key;

    if (message->key.state == RawKey::State::Pressed) {
        const auto code = message->key.keyCode;
        if (code == bsp::KeyCodes::FnRight) {
            bus.sendUnicast(message, service::name::system_manager);
        }
        backlightHandler.handleKeyPressed();
    }
    else if (message->key.state == RawKey::State::Moved) {
        handleKeyMoveEvent(message->key);
        backlightHandler.handleKeyPressed();
    }

    // send key to focused application
    if (!targetApplication.empty()) {
        bus.sendUnicast(message, targetApplication);
    }
    // notify application manager to prevent screen locking
    app::manager::Controller::preventBlockingDevice(this);
}

void EventManager::handleKeyMoveEvent(RawKey key)
{
    if (isSliderKeyCode(key.keyCode)) {
        LOG_INFO("Slider position: %s", magic_enum::enum_name(key.keyCode).data());
        const auto mode = sys::SystemManager::translateSliderState(key);
        bus.sendUnicast(std::make_shared<sys::PhoneModeRequest>(mode), service::name::system_manager);
    }
}

int EventManager::dumpLogsToFile()
{
    const auto logPath = purefs::dir::getLogsPath() / LOG_FILE_NAME;
    const auto ts      = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());

    LOG_DEBUG("Log flush timestamp: %d", static_cast<unsigned>(ts));

    return Log::Logger::get().dumpToFile(std::move(logPath));
}

void EventManager::handleMinuteUpdate(time_t timestamp)
{
    if (!targetApplication.empty()) {
        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        bus.sendUnicast(message, targetApplication);
    }
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

void EventManager::processRTCFromTmRequest(struct tm &newTime)
{
    if (bsp::rtc::setDateTime(&newTime) != bsp::rtc::ErrorCode::OK) {
        LOG_ERROR("Setting RTC failed.");
        return;
    }
    auto timestamp = time(nullptr);
    bsp::rtc::setMinuteAlarm(timestamp);
    auto notification = std::make_shared<sys::DataMessage>(MessageType::EVMTimeUpdated);
    bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceEvtmgrNotifications);
}

void EventManager::processRTCFromTimestampRequest(time_t &newTime)
{
    if (bsp::rtc::setDateTimeFromTimestamp(newTime) != bsp::rtc::ErrorCode::OK) {
        LOG_ERROR("Setting RTC failed.");
        return;
    }
    bsp::rtc::setMinuteAlarm(newTime);
    auto notification = std::make_shared<sys::DataMessage>(MessageType::EVMTimeUpdated);
    bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceEvtmgrNotifications);
}

void EventManager::processTimezoneRequest(const std::string &timezone)
{
    if (setenv("TZ", timezone.c_str(), 1) != 0) {
        LOG_ERROR("Setting timezone failed.");
        return;
    }
    auto notification = std::make_shared<sys::DataMessage>(MessageType::EVMTimeUpdated);
    bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceEvtmgrNotifications);
}

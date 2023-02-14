// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/BatteryMessages.hpp"
#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/EventManagerCommon.hpp"
#include "service-evtmgr/WorkerEventCommon.hpp"
#include "service-evtmgr/AppSettingsNotify.hpp"

#include <BaseInterface.hpp>
#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <Timers/TimerFactory.hpp>
#include <system/Constants.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <bsp/common.hpp>
#include <bsp/rtc/rtc.hpp>
#include <hal/key_input/KeyEventDefinitions.hpp>
#include <log/log.hpp>
#include <Logger.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-time/Constants.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-cellular/Constans.hpp>

#include <cassert>
#include <fstream>
#include <filesystem>
#include <list>
#include <ctime>
#include <apps-common/messages/AppMessage.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>
#include <EventStore.hpp>
#include <ticks.hpp>
#include <purefs/filesystem_paths.hpp>
#include <Constants.hpp>

#if DEBUG_INPUT_EVENTS == 1
#define debug_input_events(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_input_events(...)
#endif

EventManagerCommon::EventManagerCommon(LogDumpFunction logDumpFunction, const std::string &name)
    : sys::Service(name, "", stackDepth), logDumpFunction(std::move(logDumpFunction)),
      settings(std::make_shared<settings::Settings>())
{
    LOG_INFO("[%s] Initializing", name.c_str());
    alarmTimestamp = 0;
    alarmID        = 0;
    bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
}

EventManagerCommon::~EventManagerCommon()
{
    if (EventWorker != nullptr) {
        EventWorker->close();
    }
}

// Invoked upon receiving data message
sys::MessagePointer EventManagerCommon::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    bool handled = false;

    if (msgl->messageType == MessageType::EVM_GPIO) {
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
        auto event = msg->getEvent();
        evm::api::notifySettingsBluetoothAudio(bus, event);
        AudioServiceAPI::SendEvent(this, msg->getEvent());
        handled = true;
    }

    if (handled) {
        return std::make_shared<sys::ResponseMessage>();
    }
    else {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }
}

// Invoked during initialization
sys::ReturnCodes EventManagerCommon::InitHandler()
{
    settings->init(service::ServiceProxy(shared_from_this()));

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

    connect(sevm::BatteryStatusChangeMessage(), [&](sys::Message *msgl) {
        if (msgl->sender == this->GetName()) {
            if (Store::Battery::get().state == Store::Battery::State::Discharging) {
                bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), service::name::system_manager);
            }

            if (!targetApplication.empty()) {
                bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), targetApplication);
            }
            bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), service::name::service_desktop);
            bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), service::name::bluetooth);
            bus.sendUnicast(std::make_shared<sevm::BatteryStatusChangeMessage>(), service::name::cellular);
        }
        return sys::msgHandled();
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

    initProductEvents();

    EventWorker = createEventWorker();
    EventWorker->init(settings);
    EventWorker->run();

    cpuSentinel                  = std::make_shared<sys::TimedCpuSentinel>(service::name::evt_manager, this);
    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    return sys::ReturnCodes::Success;
}

void EventManagerCommon::initProductEvents()
{}

sys::ReturnCodes EventManagerCommon::DeinitHandler()
{
    settings->deinit();

    EventWorker->close();
    EventWorker.reset();
    EventWorker = nullptr;

    return sys::ReturnCodes::Success;
}

void EventManagerCommon::ProcessCloseReasonHandler(sys::CloseReason closeReason)
{
    ProcessCloseReason(closeReason);
    EventWorker->deinitProductHardware();
    sendCloseReadyMessage(this);
}

sys::ReturnCodes EventManagerCommon::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceEvtMgr] PowerModeHandler: %s", c_str(mode));

    switch (mode) {
    case sys::ServicePowerMode::Active:
        break;
    case sys::ServicePowerMode::SuspendToRAM:
    case sys::ServicePowerMode::SuspendToNVM:
        break;
    }

    return sys::ReturnCodes::Success;
}

bool EventManagerCommon::messageSetApplication(sys::Service *sender, const std::string &applicationName)
{
    auto msg = std::make_shared<sevm::EVMFocusApplication>(applicationName);
    return sender->bus.sendUnicast(msg, service::name::evt_manager);
}

void EventManagerCommon::handleKeyEvent(sys::Message *msg)
{
    auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    auto message    = std::make_shared<sevm::KbdMessage>();
    message->key    = kbdMessage->key;

    debug_input_events("EVInput -> K:|%s|, S:|%s|, TP:|%d|, TR:|%d|, App:|%s|",
                       magic_enum::enum_name(message->key.keyCode).data(),
                       magic_enum::enum_name(message->key.state).data(),
                       message->key.timePress,
                       message->key.timeRelease,
                       targetApplication.c_str());

    // send key to focused application
    if (!targetApplication.empty()) {
        bus.sendUnicast(message, targetApplication);
    }
    else {
        debug_input_events("EventManagerInput -> No target Application!");
    }
    // notify application manager to prevent screen locking
    app::manager::Controller::preventBlockingDevice(this);
}

int EventManagerCommon::dumpLogsToFile()
{
    if (logDumpFunction) {
        return logDumpFunction();
    }
    return 0;
}

void EventManagerCommon::handleMinuteUpdate(time_t timestamp)
{
    if (onMinuteTick) {
        onMinuteTick(timestamp);
    }
    if (!targetApplication.empty()) {
        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        bus.sendUnicast(message, targetApplication);
        bus.sendUnicast(message, service::name::service_time);
    }
}

void EventManagerCommon::processRTCFromTmRequest(struct tm &newTime)
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

void EventManagerCommon::processRTCFromTimestampRequest(time_t &newTime)
{
    if (bsp::rtc::setDateTimeFromTimestamp(newTime) != bsp::rtc::ErrorCode::OK) {
        LOG_ERROR("Setting RTC failed.");
        return;
    }
    bsp::rtc::setMinuteAlarm(newTime);
    auto notification = std::make_shared<sys::DataMessage>(MessageType::EVMTimeUpdated);
    bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceEvtmgrNotifications);
}

void EventManagerCommon::processTimezoneRequest(const std::string &timezone)
{
    if (setenv("TZ", timezone.c_str(), 1) != 0) {
        LOG_ERROR("Setting timezone failed.");
        return;
    }
    auto notification = std::make_shared<sys::DataMessage>(MessageType::EVMTimeUpdated);
    bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceEvtmgrNotifications);
}

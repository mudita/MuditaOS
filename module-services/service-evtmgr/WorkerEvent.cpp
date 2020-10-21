// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

extern "C"
{
#include "FreeRTOS.h" // for xQueueHandle
#include "projdefs.h" // for pdTRUE
#include "queue.h"    // for xQueueReceive
#include "task.h"     // for xTaskGetTickC...
}

#include "Audio/AudioCommon.hpp" // for EventType
#include "Constants.hpp"         // for evt_manager
#include "MessageType.hpp"       // for MessageType
#include "Service/Worker.hpp"    // for WorkerQueueInfo
#include "WorkerEvent.hpp"
#include "bsp/battery-charger/battery_charger.hpp"     // for battery_Clear...
#include "bsp/cellular/bsp_cellular.hpp"               // for getStatus
#include "bsp/harness/bsp_harness.hpp"                 // for Init, emit
#include "bsp/keyboard/keyboard.hpp"                   // for keyboard_Deinit
#include "bsp/magnetometer/magnetometer.hpp"           // for init
#include "bsp/rtc/rtc.hpp"                             // for rtc_GetCurren...
#include "bsp/torch/torch.hpp"                         // for deinit, init
#include "bsp/vibrator/vibrator.hpp"                   // for init
#include "common_data/EventStore.hpp"                  // for GSM, GSM::Tray
#include "common_data/RawKey.hpp"                      // for RawKey, RawKe...
#include "harness/Parser.hpp"                          // for c_str, parse
#include "headset.hpp"                                 // for Deinit, Handler
#include "log/log.hpp"                                 // for LOG_ERROR
#include "service-audio/messages/AudioMessage.hpp"     // for AudioEventReq...
#include "service-evtmgr/messages/BatteryMessages.hpp" // for BatteryLevelM...
#include "service-evtmgr/messages/EVMessages.hpp"      // for StatusStateMe...
#include "service-evtmgr/messages/KbdMessage.hpp"      // for KbdMessage

#include <Service/Bus.hpp> // for Bus
#include <sys/types.h>     // for time_t

#include <memory> // for make_shared
#include <string> // for string
#include <vector> // for vector

bool WorkerEvent::handleMessage(uint32_t queueID)
{

    xQueueHandle queue = queues[queueID];

    // service queue
    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueService)) {
        sys::WorkerCommand wcmd;
        if (xQueueReceive(queue, &wcmd, 0) != pdTRUE) {
            return false;
        }
        wcmd.command = 1;
        // place some code here to handle messages from service
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueKeyboardIRQ)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }
        uint8_t state, code;
        bsp::keyboard_get_data(notification, state, code);

        processKeyEvent(static_cast<bsp::KeyEvents>(state), static_cast<bsp::KeyCodes>(code));
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueHeadsetIRQ)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }

        if (bsp::headset::Handler(notification) == true) {
            bool state = bsp::headset::IsInserted();
            auto message = std::make_shared<AudioEventRequest>(state ? audio::EventType::HeadphonesPlugin
                                                                     : audio::EventType::HeadphonesUnplug);
            sys::Bus::SendUnicast(message, service::name::evt_manager, this->service);
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueBattery)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }
        if (notification & static_cast<uint8_t>(bsp::batteryIRQSource::INTB)) {
            uint8_t battLevel = 0;
            bsp::battery_getBatteryLevel(battLevel);
            bsp::battery_ClearAllIRQs();
            auto message           = std::make_shared<sevm::BatteryLevelMessage>();
            message->levelPercents = battLevel;
            message->fullyCharged  = false;
            sys::Bus::SendUnicast(message, service::name::evt_manager, this->service);
        }
        if (notification & static_cast<uint8_t>(bsp::batteryIRQSource::INOKB)) {
            bool status;
            bsp::battery_getChargeStatus(status);
            bsp::battery_ClearAllIRQs();
            auto message     = std::make_shared<sevm::BatteryPlugMessage>();
            message->plugged = status;
            sys::Bus::SendUnicast(message, service::name::evt_manager, this->service);
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueRTC)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }

        time_t timestamp;
        bsp::rtc_GetCurrentTimestamp(&timestamp);
        bsp::rtc_SetMinuteAlarm(timestamp);

        struct tm time;

        bsp::rtc_GetCurrentDateTime(&time);

        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        sys::Bus::SendUnicast(message, service::name::evt_manager, this->service);
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueHarness)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }
        if (notification == STX) {
            if (bsp::harness::flush() != true) {
                LOG_ERROR("processing in progres...");
            }
        }
        else if (notification == ETX) {
            std::string text = bsp::harness::read();
            auto ret         = harness::parse(text, this->service);
            if (ret != harness::Error::Success) {
                LOG_ERROR("Harness parser error: %s", c_str(ret));
            }
        }
        else {
            LOG_ERROR("Unknown event!");
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueCellular)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }

        if (notification == bsp::cellular::statusPin) {
            auto GSMstatus = bsp::cellular::status::getStatus();
            LOG_DEBUG("GSM Status pin change: %s",
                      (GSMstatus == bsp::cellular::status::value::ACTIVE ? "ACTIVE" : "INACTIVE"));

            auto message   = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMModemStatus);
            message->state = GSMstatus;
            sys::Bus::SendUnicast(message, "EventManager", this->service);
        }

        if (notification == bsp::cellular::trayPin) {
            Store::GSM::Tray pinstate = bsp::cellular::sim::getTray();
            LOG_DEBUG("SIM state change: %d", static_cast<int>(pinstate));
            bsp::cellular::sim::hotswap_trigger();
        }

        if (notification == bsp::cellular::ringIndicatorPin) {
            auto message = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMRingIndicator);
            sys::Bus::SendUnicast(message, "EventManager", this->service);
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueMagnetometerIRQ)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }

        if (std::optional<bsp::KeyCodes> key = bsp::magnetometer::WorkerEventHandler()) {
            LOG_DEBUG("magneto IRQ handler: %s", c_str(*key));
            processKeyEvent(bsp::KeyEvents::Pressed, *key);
        }
    }

    return true;
}

#include "harness/events/SysStart.hpp" // for SysStart

bool WorkerEvent::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);
    std::vector<xQueueHandle> qhandles = this->queues;
    bsp::vibrator::init();
    bsp::keyboard_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueKeyboardIRQ)]);
    bsp::headset::Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueHeadsetIRQ)]);
    bsp::battery_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueBattery)]);
    bsp::rtc_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueRTC)]);
    bsp::harness::Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueHarness)]);
    bsp::cellular::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueCellular)]);
    bsp::magnetometer::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueMagnetometerIRQ)]);
    bsp::torch::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueTorch)]);

    time_t timestamp;
    bsp::rtc_GetCurrentTimestamp(&timestamp);
    bsp::rtc_SetMinuteAlarm(timestamp);
    bsp::harness::emit(harness::SysStart().encode());
    return true;
}

bool WorkerEvent::deinit(void)
{
    Worker::deinit();
    bsp::keyboard_Deinit();
    bsp::headset::Deinit();
    bsp::battery_Deinit();
    bsp::torch::deinit();

    return true;
}

void WorkerEvent::processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code)
{
    auto message = std::make_shared<sevm::KbdMessage>();

    message->key.key_code = code;

    if (event == bsp::KeyEvents::Pressed) {
        if (lastState == bsp::KeyEvents::Pressed) {
            return;
        }

        message->key.state      = RawKey::State::Pressed;
        message->key.time_press = xTaskGetTickCount();

        // Slider sends only press, not release state so it would block the entire keyboard
        if ((code != bsp::KeyCodes::SSwitchUp) && (code != bsp::KeyCodes::SSwitchMid) &&
            (code != bsp::KeyCodes::SSwitchDown)) {
            lastPressed = code;
            lastState   = event;
        }
    }
    else {
        if (lastState != bsp::KeyEvents::Pressed) {
            return;
        }
        if (lastPressed != code) {
            return;
        }

        lastState = bsp::KeyEvents::Released;
        {
            message->key.state        = RawKey::State::Released;
            message->key.time_release = xTaskGetTickCount();
        }
    }
    sys::Bus::SendUnicast(message, service::name::evt_manager, this->service);
}

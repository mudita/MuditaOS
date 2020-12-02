// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/BatteryMessages.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/WorkerEvent.hpp"

#include <Audio/AudioCommon.hpp>
#include <MessageType.hpp>
#include <Service/Bus.hpp>
#include <Service/Worker.hpp>
#include <bsp/battery-charger/battery_charger.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/keyboard/keyboard.hpp>
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <bsp/vibrator/vibrator.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <common_data/EventStore.hpp>
#include <common_data/RawKey.hpp>
#include <headset.hpp>
#include <log/log.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>

extern "C"
{
#include "FreeRTOS.h" // for xQueueHandle
#include "projdefs.h" // for pdTRUE
#include "queue.h"    // for xQueueReceive, QueueDefinition, QueueHandle_t
#include "task.h"     // for xTaskGetTickCount
}

#include <sys/types.h> // for time_t
#include <memory>      // for make_shared, __shared_ptr_access, shared_ptr, allocator
#include <optional>    // for optional
#include <string>      // for string
#include <vector>      // for vector

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
            auto message = std::make_shared<AudioEventRequest>(audio::EventType::JackState,
                                                               state ? audio::Event::DeviceState::Connected
                                                                     : audio::Event::DeviceState::Disconnected);
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

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueLightSensor)) {
        uint8_t notification;
        if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
            return false;
        }

        LOG_DEBUG("Light sensor IRQ");
        bsp::light_sensor::readout();
    }

    return true;
}

bool WorkerEvent::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);
    std::vector<xQueueHandle> qhandles = this->queues;
    bsp::vibrator::init();
    bsp::keyboard_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueKeyboardIRQ)]);
    bsp::headset::Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueHeadsetIRQ)]);
    bsp::battery_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueBattery)]);
    bsp::rtc_Init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueRTC)]);
    bsp::cellular::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueCellular)]);
    bsp::magnetometer::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueMagnetometerIRQ)]);
    bsp::torch::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueTorch)]);
    bsp::keypad_backlight::init();
    bsp::eink_frontlight::init();
    bsp::light_sensor::init(qhandles[static_cast<int32_t>(WorkerEventQueues::queueLightSensor)]);

    time_t timestamp;
    bsp::rtc_GetCurrentTimestamp(&timestamp);
    bsp::rtc_SetMinuteAlarm(timestamp);

    return true;
}

bool WorkerEvent::deinit(void)
{
    Worker::deinit();
    bsp::keyboard_Deinit();
    bsp::headset::Deinit();
    bsp::battery_Deinit();
    bsp::torch::deinit();
    bsp::keypad_backlight::deinit();
    bsp::eink_frontlight::deinit();
    bsp::light_sensor::deinit();

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

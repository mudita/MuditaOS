﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/WorkerEventCommon.hpp"
#include "battery-level-check/BatteryLevelCheck.hpp"

#include <Audio/AudioCommon.hpp>
#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/keyboard/keyboard.hpp>
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <bsp/vibrator/vibrator.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <EventStore.hpp>

#include <common_data/RawKey.hpp>
#include <headset.hpp>
#include <log.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <SystemManager/messages/SentinelRegistrationMessage.hpp>

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

WorkerEventCommon::WorkerEventCommon(sys::Service *service)
    : sys::Worker(service, stackDepthBytes),
      service(service), batteryCharger{hal::battery::AbstractBatteryCharger::Factory::create(service)}
{}

bool WorkerEventCommon::handleMessage(uint32_t queueID)
{
    auto &queue = queues[queueID];

    // service queue
    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueService)) {
        sys::WorkerCommand wcmd;
        if (!queue->Dequeue(&wcmd, 0)) {
            return false;
        }
        wcmd.command = 1;
        // place some code here to handle messages from service
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueKeyboardIRQ)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        for (const auto &key : bsp::keyboard::getKeyEvents(notification)) {
            processKeyEvent(key.event, key.code);
        }
    }

    if (queueID == static_cast<std::uint32_t>(WorkerEventQueues::queueBattery)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        batteryCharger->processStateChangeNotification(notification);
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueChargerDetect)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        LOG_DEBUG("USB charger type: %d", notification);
        batteryCharger->setChargingCurrentLimit(notification);
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueRTC)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        time_t timestamp;
        bsp::rtc::getCurrentTimestamp(&timestamp);
        bsp::rtc::setMinuteAlarm(timestamp);

        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        service->bus.sendUnicast(message, service::name::evt_manager);
    }


    return true;
}

void WorkerEventCommon::addProductQueues(std::list<sys::WorkerQueueInfo> &queueList)
{}

void WorkerEventCommon::initProductHardware()
{}

void WorkerEventCommon::deinitProductHardware()
{}

bool WorkerEventCommon::initEventQueues()
{
    constexpr auto elementSize = sizeof(std::uint8_t);
    std::list<sys::WorkerQueueInfo> queuesList;

    queuesList.emplace_back(keyboardQueueName, elementSize, keyboardQueueSize);
    queuesList.emplace_back(batteryQueueName, elementSize, batteryQueueSize);
    queuesList.emplace_back(chargerQueueName, elementSize, rtcQueueSize);
    queuesList.emplace_back(rtcQueueName, elementSize, rtcQueueSize);

    addProductQueues(queuesList);

    return Worker::init(queuesList);
}

bool WorkerEventCommon::initCommonHardwareComponents()
{
    bsp::keyboard::init(queues[static_cast<int32_t>(WorkerEventQueues::queueKeyboardIRQ)]->GetQueueHandle());
    auto queueBatteryHandle = queues[static_cast<int32_t>(WorkerEventQueues::queueBattery)]->GetQueueHandle();
    auto queueChargerDetect = queues[static_cast<int32_t>(WorkerEventQueues::queueChargerDetect)]->GetQueueHandle();
    batteryCharger->init(queueBatteryHandle, queueChargerDetect);
    bsp::rtc::init(queues[static_cast<int32_t>(WorkerEventQueues::queueRTC)]->GetQueueHandle());

    time_t timestamp;
    bsp::rtc::getCurrentTimestamp(&timestamp);
    bsp::rtc::setMinuteAlarm(timestamp);

    cpuSentinel = std::make_shared<sys::CpuSentinel>(
        service::name::evt_manager, service, [this](bsp::CpuFrequencyHz newFrequency) {
            updateResourcesAfterCpuFrequencyChange(newFrequency);
        });

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    service->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

    initProductHardware();

    return true;
}

void WorkerEventCommon::init(std::shared_ptr<settings::Settings> settings)
{
    initEventQueues();
    initCommonHardwareComponents();
    battery_level_check::init(service, settings);
}

bool WorkerEventCommon::deinit(void)
{
    Worker::deinit();

    deinitProductHardware();

    bsp::keyboard::deinit();
    batteryCharger->deinit();
    battery_level_check::deinit();

    return true;
}

void WorkerEventCommon::processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code)
{
    auto message = std::make_shared<sevm::KbdMessage>();

    message->key.keyCode = code;

    switch (event) {
    case bsp::KeyEvents::Pressed:
        if (lastState == bsp::KeyEvents::Pressed) {
            return;
        }
        message->key.state     = RawKey::State::Pressed;
        message->key.timePress = xTaskGetTickCount();
        lastPressed            = code;
        lastState              = event;
        break;
    case bsp::KeyEvents::Released:
        if (lastState != bsp::KeyEvents::Pressed) {
            return;
        }
        if (lastPressed != code) {
            return;
        }
        lastState = bsp::KeyEvents::Released;
        {
            message->key.state       = RawKey::State::Released;
            message->key.timeRelease = xTaskGetTickCount();
        }
        break;
    case bsp::KeyEvents::Moved:
        message->key.state = RawKey::State::Moved;
        break;
    }
    service->bus.sendUnicast(message, service::name::evt_manager);
}

void WorkerEventCommon::updateResourcesAfterCpuFrequencyChange(bsp::CpuFrequencyHz newFrequency)
{
    bsp::eink_frontlight::updateClockFrequency(newFrequency);
}

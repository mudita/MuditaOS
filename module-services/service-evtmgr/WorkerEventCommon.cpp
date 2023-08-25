﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/ServiceEventManagerName.hpp"
#include "service-evtmgr/WorkerEventCommon.hpp"
#include "battery/BatteryController.hpp"

#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/vibrator/vibrator.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>

#include <hal/key_input/RawKey.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

#include "task.h"

#include <memory>
#include <optional>

#include <log/log.hpp>
#include <memory/usermem.h>

WorkerEventCommon::WorkerEventCommon(sys::Service *service)
    : sys::Worker(service, stackDepthBytes),
      service(service), keyInput{hal::key_input::AbstractKeyInput::Factory::create()}
{}

bool WorkerEventCommon::handleMessage(std::uint32_t queueID)
{
#if DEBUG_HEAP_ALLOCATIONS == 1
    LOG_INFO("Free space: %zu", usermemGetFreeHeapSize());
#endif

    auto &queue = queues[queueID];

    // service queue
    if (queueID == static_cast<std::uint32_t>(WorkerEventQueues::queueService)) {
        sys::WorkerCommand wcmd;
        if (!queue->Dequeue(&wcmd, 0)) {
            return false;
        }
        wcmd.command = 1;
        // place some code here to handle messages from service
    }

    if (queueID == static_cast<std::uint32_t>(WorkerEventQueues::queueKeyboardIRQ)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        for (const auto &key : keyInput->getKeyEvents(notification)) {
            processKeyEvent(key.event, key.code);
        }
    }

    if (queueID == static_cast<std::uint32_t>(WorkerEventQueues::queueBatteryController)) {
        sevm::battery::BatteryController::Events event;
        if (!queue->Dequeue(&event, 0)) {
            return false;
        }

        batteryController->handleNotification(event);
    }

    if (queueID == static_cast<std::uint32_t>(WorkerEventQueues::queueRTC)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        std::time_t timestamp;
        bsp::rtc::getCurrentTimestamp(&timestamp);
        bsp::rtc::setMinuteAlarm(timestamp);

        /// Poll battery controller to recalculate state and possibly send update requests to
        /// appmgr/sysmgr
        batteryController->poll();

        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        service->bus.sendUnicast(std::move(message), service::name::evt_manager);
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
    queuesList.emplace_back(rtcQueueName, elementSize, rtcQueueSize);

    addProductQueues(queuesList);

    return Worker::init(queuesList);
}

bool WorkerEventCommon::initCommonHardwareComponents(EventManagerParams params)
{
    keyInput->init(queues[static_cast<std::int32_t>(WorkerEventQueues::queueKeyboardIRQ)]->GetQueueHandle());

    const auto queueBatteryHandle =
        queues[static_cast<std::int32_t>(WorkerEventQueues::queueBatteryController)]->GetQueueHandle();
    batteryController = std::make_shared<sevm::battery::BatteryController>(service, queueBatteryHandle, params);

    std::time_t timestamp;
    bsp::rtc::init(queues[static_cast<std::int32_t>(WorkerEventQueues::queueRTC)]->GetQueueHandle());
    bsp::rtc::getCurrentTimestamp(&timestamp);
    bsp::rtc::setMinuteAlarm(timestamp);

    cpuSentinel = std::make_shared<sys::CpuSentinel>("WorkerEvent", service, [this](bsp::CpuFrequencyMHz newFrequency) {
        updateResourcesAfterCpuFrequencyChange(newFrequency);
    });

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    service->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

    initProductHardware();

    return true;
}

void WorkerEventCommon::init(std::shared_ptr<settings::Settings> settings, EventManagerParams params)
{
    initEventQueues();
    initCommonHardwareComponents(params);
}

bool WorkerEventCommon::deinit()
{
    Worker::deinit();
    keyInput->deinit();
    return true;
}

void WorkerEventCommon::sendKeyUnicast(RawKey const &key)
{
    auto message = std::make_shared<sevm::KbdMessage>();
    message->key = key;
    service->bus.sendUnicast(std::move(message), service::name::evt_manager);
}

void WorkerEventCommon::processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code)
{
    switch (event) {
    case bsp::KeyEvents::Pressed: {
        auto const tick = xTaskGetTickCount();
        if (lastState == bsp::KeyEvents::Pressed) {
            LOG_WARN("Generating release %s", c_str(lastPressed));
            sendKeyUnicast({RawKey::State::Released, lastPressed, 0, tick});
        }
        sendKeyUnicast({RawKey::State::Pressed, code, tick, 0});
        lastState   = bsp::KeyEvents::Pressed;
        lastPressed = code;
        break;
    }

    case bsp::KeyEvents::Released:
        if (lastState != bsp::KeyEvents::Pressed || lastPressed != code) {
            return;
        }
        sendKeyUnicast({RawKey::State::Released, code, 0, xTaskGetTickCount()});
        lastState   = bsp::KeyEvents::Released;
        lastPressed = code;
        break;

    case bsp::KeyEvents::Moved:
        sendKeyUnicast({RawKey::State::Moved, code});
        break;
    }
}

void WorkerEventCommon::updateResourcesAfterCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency)
{
    bsp::eink_frontlight::updateClockFrequency(newFrequency);
    bsp::vibrator::updateClockFrequency(newFrequency);
}

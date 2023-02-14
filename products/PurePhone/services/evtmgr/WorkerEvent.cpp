// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"

#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <bsp/vibrator/vibrator.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <headset.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-evtmgr/EVMessages.hpp>

using purephone::WorkerEvent;

WorkerEvent::WorkerEvent(sys::Service *service) : WorkerEventCommon(service)
{}

void WorkerEvent::addProductQueues(std::list<sys::WorkerQueueInfo> &queuesList)
{
    constexpr auto elementSize = sizeof(std::uint8_t);
    queuesList.emplace_back(headsetQueueName, elementSize, headsetQueueSize);
    queuesList.emplace_back(simNotifyQueueName, elementSize, simNotifyQueueSize);
    queuesList.emplace_back(magnetometerQueueName, elementSize, magnetometerQueueSize);
    queuesList.emplace_back(magnetometerNotifyQueueName, elementSize, magnetometerNotifyQueueSize);
}

void WorkerEvent::initProductHardware()
{
    bsp::cellular::init(queues[static_cast<std::int32_t>(EventQueues::queueCellular)]->GetQueueHandle());
    bsp::headset::Init(queues[static_cast<std::int32_t>(EventQueues::queueHeadsetIRQ)]->GetQueueHandle());
    bsp::magnetometer::init(queues[static_cast<std::int32_t>(EventQueues::queueMagnetometerIRQ)]->GetQueueHandle());
    bsp::torch::init();
    bsp::light_sensor::init();
    bsp::keypad_backlight::init();
    bsp::eink_frontlight::init();
    bsp::vibrator::init();
}

bool WorkerEvent::handleMessage(std::uint32_t queueID)
{
    auto &queue = queues[queueID];

    if (queueID == static_cast<std::uint32_t>(EventQueues::queueHeadsetIRQ)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        bool headsetState, microphoneState;
        std::uint8_t headsetKeyEvent, headsetKeyCode;

        if (bsp::headset::headset_get_data(headsetState, microphoneState, headsetKeyEvent, headsetKeyCode) ==
            bsp::headset::HeadsetState::Changed) {

            auto jackStateMessage = std::make_shared<AudioEventRequest>(
                audio::EventType::JackState,
                headsetState ? audio::Event::DeviceState::Connected : audio::Event::DeviceState::Disconnected);
            service->bus.sendUnicast(std::move(jackStateMessage), service::name::evt_manager);

            auto micStateMessage = std::make_shared<AudioEventRequest>(
                audio::EventType::MicrophoneState,
                microphoneState ? audio::Event::DeviceState::Connected : audio::Event::DeviceState::Disconnected);
            service->bus.sendUnicast(std::move(micStateMessage), service::name::evt_manager);
        }
        else if (auto keyCode = headsetKeyToKeyboardKey(headsetKeyCode); keyCode != bsp::KeyCodes::Undefined) {
            WorkerEventCommon::processKeyEvent(static_cast<bsp::KeyEvents>(headsetKeyEvent), keyCode);
        }
    }

    if (queueID == static_cast<std::uint32_t>(EventQueues::queueCellular)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        if (notification == bsp::cellular::statusPin) {
            auto GSMstatus = bsp::cellular::status::getStatus();
            LOG_DEBUG("GSM Status pin change: %s",
                      (GSMstatus == bsp::cellular::status::value::ACTIVE ? "ACTIVE" : "INACTIVE"));

            auto message   = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMModemStatus);
            message->state = GSMstatus;
            service->bus.sendUnicast(message, "EventManager");
        }

        if (notification == bsp::cellular::trayPin) {
            Store::GSM::Tray pinstate = bsp::cellular::sim::getTray();
            LOG_DEBUG("SIM state change: %d", static_cast<int>(pinstate));
            service->bus.sendUnicast(std::make_shared<sevm::SIMTrayMessage>(), service::name::evt_manager);
        }

        if (notification == bsp::cellular::ringIndicatorPin) {
            auto message = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMRingIndicator);
            service->bus.sendUnicast(message, "EventManager");
        }
    }

    if (queueID == static_cast<std::uint32_t>(EventQueues::queueMagnetometerNotify)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        bsp::magnetometer::resetCurrentParsedValue();
        LOG_WARN("Received notify, current value reset!");
        handleMagnetometerEvent();
    }

    if (queueID == static_cast<std::uint32_t>(EventQueues::queueMagnetometerIRQ)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        handleMagnetometerEvent();
    }

    return WorkerEventCommon::handleMessage(queueID);
}

void WorkerEvent::deinitProductHardware()
{
    /* Deinitialize in reverse order */
    bsp::vibrator::deinit();
    bsp::eink_frontlight::deinit();
    bsp::keypad_backlight::deinit();
    bsp::light_sensor::deinit();
    bsp::torch::deinit();
    bsp::magnetometer::deinit();
    bsp::headset::Deinit();
}

void WorkerEvent::requestSliderPositionRead()
{
    std::uint8_t request = 1;
    if (auto queue = getQueueByName(WorkerEvent::magnetometerNotifyQueueName); !queue->Overwrite(&request)) {
        LOG_ERROR("Unable to overwrite the request.");
    }
}

void WorkerEvent::handleMagnetometerEvent()
{
    if (const auto &key = bsp::magnetometer::WorkerEventHandler(); key.has_value()) {
        LOG_DEBUG("magneto IRQ handler: %s", c_str(*key));
        processKeyEvent(bsp::KeyEvents::Moved, *key);
    }
}

std::shared_ptr<sevm::battery::BatteryController> WorkerEvent::createBatteryController(sys::Service *service,
                                                                                       QueueHandle_t queue)
{
    return std::make_shared<sevm::battery::BatteryController>(service, queue);
}

bsp::KeyCodes WorkerEvent::headsetKeyToKeyboardKey(std::uint8_t headsetKeyCode)
{
    switch (headsetKeyCode) {
    case static_cast<std::uint8_t>(bsp::headset::KeyCode::Key1):
        return bsp::KeyCodes::HeadsetOk;

    case static_cast<std::uint8_t>(bsp::headset::KeyCode::Key3):
        return bsp::KeyCodes::HeadsetVolUp;

    case static_cast<std::uint8_t>(bsp::headset::KeyCode::Key4):
        return bsp::KeyCodes::HeadsetVolDown;
    }
    return bsp::KeyCodes::Undefined;
}

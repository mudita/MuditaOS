// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"

#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <bsp/rotary_encoder/rotary_encoder.hpp>

namespace bell
{

    WorkerEvent::WorkerEvent(sys::Service *service) : WorkerEventCommon(service)
    {}

    void WorkerEvent::addProductQueues(std::list<sys::WorkerQueueInfo> &queuesList)
    {
        constexpr auto elementSize = sizeof(std::uint8_t);
        queuesList.emplace_back(rotaryEncoderQueueName, elementSize, rotaryEncoderQueueSize);
    }

    void WorkerEvent::initProductHardware()
    {
        bsp::eink_frontlight::init();
        bsp::rotary_encoder::init(queues[static_cast<int32_t>(EventQueues::queueRotaryEncoder)]->GetQueueHandle());
    }

    bool WorkerEvent::handleMessage(std::uint32_t queueID)
    {
        auto &queue = queues[queueID];
        if (queueID == static_cast<uint32_t>(EventQueues::queueRotaryEncoder)) {
            uint8_t notification;
            if (!queue->Dequeue(&notification, 0)) {
                return false;
            }
            handleRotaryEncoderEvent();
        }
        return WorkerEventCommon::handleMessage(queueID);
    }

    void WorkerEvent::deinitProductHardware()
    {
        bsp::eink_frontlight::deinit();
        bsp::rotary_encoder::deinit();
    }

    void WorkerEvent::processRotaryAsShortRelease(bsp::KeyCodes code)
    {
        processKeyEvent(bsp::KeyEvents::Pressed, code);
        processKeyEvent(bsp::KeyEvents::Released, code);
    }

    void WorkerEvent::handleRotaryEncoderEvent()
    {
        if (const auto &key = bsp::rotary_encoder::WorkerEventHandler(); key.has_value()) {
            if (key.value() == bsp::rotary_encoder::type::forward) {
                processRotaryAsShortRelease(bsp::KeyCodes::JoystickUp);
            }
            else if (key.value() == bsp::rotary_encoder::type::backward) {
                processRotaryAsShortRelease(bsp::KeyCodes::JoystickDown);
            }
            else {
                LOG_ERROR("Unknown rotary event");
            }
        }
    }
    void WorkerEvent::processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code)
    {
        auto message = std::make_shared<sevm::KbdMessage>();

        message->key.keyCode = code;

        switch (event) {
        case bsp::KeyEvents::Pressed:
            message->key.state     = RawKey::State::Pressed;
            message->key.timePress = xTaskGetTickCount();
            break;
        case bsp::KeyEvents::Released:
            message->key.state       = RawKey::State::Released;
            message->key.timeRelease = xTaskGetTickCount();
            break;
        case bsp::KeyEvents::Moved:
            message->key.state = RawKey::State::Moved;
            break;
        }
        service->bus.sendUnicast(message, service::name::evt_manager);
    }
} // namespace bell

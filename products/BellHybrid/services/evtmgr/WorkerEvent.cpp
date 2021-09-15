// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"

#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <bsp/rotary_encoder/rotary_encoder.hpp>
#include <bsp/audio-bell/CodecAW8898.hpp>

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

        CodecParamsAW8898 codecParams;

        codecParams.sampleRate = CodecParamsAW8898::ValToSampleRate(44100);
        if (codecParams.sampleRate == CodecParamsAW8898::SampleRate::Invalid) {
            LOG_ERROR("Unsupported sample rate");
        }

        codecParams.outVolume  = 30.0;

        CodecAW8898 codec;
        codec.Start(codecParams);
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
} // namespace bell

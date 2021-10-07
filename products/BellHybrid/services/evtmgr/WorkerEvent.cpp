// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"

#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-evtmgr/EVMessages.hpp>

namespace bell
{

    WorkerEvent::WorkerEvent(sys::Service *service) : WorkerEventCommon(service)
    {}

    void WorkerEvent::addProductQueues(std::list<sys::WorkerQueueInfo> &queuesList)
    {}

    void WorkerEvent::initProductHardware()
    {
        bsp::eink_frontlight::init();
    }

    bool WorkerEvent::handleMessage(std::uint32_t queueID)
    {
        return WorkerEventCommon::handleMessage(queueID);
    }

    void WorkerEvent::deinitProductHardware()
    {
        bsp::eink_frontlight::deinit();
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

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
} // namespace bell

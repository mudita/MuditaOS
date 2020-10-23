// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string.h>
#include <stdint.h> // for uint32_t
#include <list>     // for list
#include <string>   // for allocator, string

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "parser/ParserFSM.hpp"

#include "bsp/usb/usb.hpp"

namespace sys
{
    class Service;
} // namespace sys

class WorkerDesktop : public sys::Worker
{
  private:
  public:
    const std::string RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    const std::string SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";

    WorkerDesktop(sys::Service *ownerServicePtr)
        : sys::Worker(ownerServicePtr), ownerService(ownerServicePtr), parser(ownerServicePtr){};
    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool handleMessage(uint32_t queueID) override final;
    sys::Service *ownerService = nullptr;
    parserFSM::StateMachine parser;
};

// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "log/log.hpp"
#include <FreeRTOS.h>
#include <queue.h>
#include <mutex.hpp>

class WorkerLock
{
    QueueHandle_t queue;
    cpp_freertos::MutexStandard &mutex;

  public:
    WorkerLock(QueueHandle_t queue, cpp_freertos::MutexStandard &mutex) : queue(queue), mutex(mutex)
    {}

    bool wait()
    {
        mutex.Unlock();
        auto ret = xQueueReceive(queue, nullptr, portMAX_DELAY) != pdTRUE;
        mutex.Lock();
        return ret;
    }

    bool wait(TickType_t ticks)
    {
        mutex.Unlock();
        auto ret = xQueueReceive(queue, nullptr, ticks) != pdTRUE;
        mutex.Lock();
        return ret;
    }

    bool signal()
    {
        return xQueueSend(queue, nullptr, portMAX_DELAY);
    }
};

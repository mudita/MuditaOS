// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <functional>
#include <utility>

/*
 * Worker queue implementation is used to defer work from interrupt routines.
 * This way, it is possible to keep interrupt routines short but at the same time process more intensive tasks.
 * The design goal was to use as few resources as possible, hence it is a lightweight mechanism.
 * The stack assigned to the queue worker handler is relatively small.
 * It is not suitable for performing stack-consuming tasks, for instance handling I/O.
 * If necessary, stack size can be manually reconfigured upon creation.
 */
template <typename Message> class WorkerQueue
{
    static constexpr auto minimalStackSizeInBytes = configMINIMAL_STACK_SIZE * sizeof(std::uint32_t);

  public:
    using WorkerHandle = std::function<void(const Message &)>;

    WorkerQueue(const char *name, WorkerHandle workerHandle, std::uint32_t stackSize = minimalStackSizeInBytes);
    ~WorkerQueue();

    WorkerQueue(const WorkerQueue &) = delete;
    WorkerQueue &operator=(const WorkerQueue &) = delete;
    WorkerQueue &operator=(WorkerQueue &&) = delete;

    BaseType_t post(const Message &msg);

  private:
    struct InternalMessage
    {
        Message msg;
        bool kill{false};
    };

    static constexpr auto queueLength = 2;

    xQueueHandle queueHandle{};
    xTaskHandle taskHandle{};
    WorkerHandle workerHandle{};

    void worker();
};

template <typename Message>
WorkerQueue<Message>::WorkerQueue(const char *name, WorkerHandle workerHandle, const std::uint32_t stackSize)
    : workerHandle{std::move(workerHandle)}
{
    queueHandle = xQueueCreate(queueLength, sizeof(InternalMessage));
    xTaskCreate(
        [](void *pvp) {
            WorkerQueue *inst = static_cast<WorkerQueue *>(pvp);
            inst->worker();
        },
        name,
        stackSize / sizeof(std::uint32_t),
        this,
        0,
        &taskHandle);
}

template <typename Message> WorkerQueue<Message>::~WorkerQueue()
{
    if (queueHandle && taskHandle) {
        InternalMessage killMsg{.kill = true};
        xQueueSend(queueHandle, &killMsg, pdMS_TO_TICKS(100));
    }
}
template <typename Message> void WorkerQueue<Message>::worker()
{
    while (true) {
        InternalMessage msg{};
        xQueueReceive(queueHandle, &msg, portMAX_DELAY);

        if (msg.kill) {
            vTaskDelete(nullptr);
        }
        else {
            workerHandle(msg.msg);
        }
    }
}
template <typename Message> BaseType_t WorkerQueue<Message>::post(const Message &msg)
{
    BaseType_t notifyRequired{};
    const InternalMessage postMsg{msg, false};
    xQueueSendFromISR(queueHandle, &postMsg, &notifyRequired);
    return notifyRequired;
}

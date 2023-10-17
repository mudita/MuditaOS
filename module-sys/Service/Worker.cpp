// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/Worker.hpp>

#include "FreeRTOS.h"
#include "task.h"

#include <log/log.hpp>

#include <string>
#include <utility>
#include <cassert>

namespace sys
{
    std::uint32_t Worker::count = 0;

    void Worker::taskAdapter(void *taskParam)
    {
        Worker *worker = static_cast<Worker *>(taskParam);
        worker->setState(State::Running);
        worker->task();
    }

    bool Worker::handleControlMessage()
    {
        auto receivedMessage = static_cast<std::uint8_t>(Worker::ControlMessage::Stop);
        getControlQueue().Dequeue(&receivedMessage, 0);
        LOG_INFO("Handle control message: %u", receivedMessage);
        assert(receivedMessage < controlMessagesCount);

        switch (static_cast<Worker::ControlMessage>(receivedMessage)) {
        case ControlMessage::Stop: // stop the thread
            setState(State::Stopping);
            break;

        default:
            LOG_FATAL("Unexpected control message %d received", receivedMessage);
            return false;
        }

        return true;
    }

    void Worker::task()
    {
        QueueSetMemberHandle_t activeMember;
        assert(getState() == State::Running);

        while (getState() == State::Running) {
            activeMember = xQueueSelectFromSet(queueSet, portMAX_DELAY);

            // handle control messages from parent service
            if (activeMember == getControlQueue().GetQueueHandle()) {
                handleControlMessage();
                continue;
            }

            // find id of the queue that was activated
            for (std::uint32_t i = 0; i < queues.size(); i++) {
                if (queues[i]->GetQueueHandle() == activeMember) {
                    handleMessage(i);
                }
            }
        }

        // inform about thread end and wait for the deletion
        xSemaphoreGive(joinSemaphore);
        setState(State::Stopped);
        vTaskDelete(nullptr);
    }

    Worker::Worker(sys::Service *service, std::uint16_t stackDepth)
        : name(service->GetName()), priority(service->GetPriority()), stackDepth(stackDepth)
    {
        constructName();
    }

    Worker::Worker(std::string workerNamePrefix, UBaseType_t priority, std::uint16_t stackDepth)
        : name(std::move(workerNamePrefix)), priority(priority), stackDepth(stackDepth)
    {
        constructName();
    }

    Worker::~Worker()
    {
        if (state != State::Destroyed) {
            LOG_FATAL("Calling destructor of an undestroyed worker.");
        }
    }

    void Worker::constructName()
    {
        // assign worker id
        taskENTER_CRITICAL();
        id = count++;
        taskEXIT_CRITICAL();

        name.append("_w" + std::to_string(id));
    }

    size_t Worker::addQueue(const std::string &qName, UBaseType_t maxItems, UBaseType_t itemSize)
    {
        auto idx = queues.size();
        queues.push_back(std::make_shared<WorkerQueue>(qName, maxItems, itemSize));
        vQueueAddToRegistry(queues.back()->GetQueueHandle(), qName.c_str());
        return idx;
    }

    WorkerQueue &Worker::getControlQueue() const
    {
        assert(controlQueueIndex);
        return *queues.at(controlQueueIndex.value());
    }

    WorkerQueue &Worker::getServiceQueue() const
    {
        assert(serviceQueueIndex);
        return *queues.at(serviceQueueIndex.value());
    }

    inline std::string Worker::getControlQueueName() const
    {
        return controlQueueNamePrefix + std::to_string(id);
    }

    bool Worker::init(std::list<WorkerQueueInfo> queuesList)
    {
        assert(state == State::New);

        // initial value is because there is always a service and control queue
        // to communicate with the parent service
        auto setSize = SERVICE_QUEUE_LENGTH + CONTROL_QUEUE_LENGTH;

        // iterate over all entries in the list of queues and summarize queue sizes
        for (const auto &wqi : queuesList) {
            setSize += wqi.length;
        }

        // create set of queues
        queueSet = xQueueCreateSet(setSize);
        if (queueSet == nullptr) {
            state = State::Invalid;
            return false;
        }

        // create and add all queues to the set. First service queue is created.
        serviceQueueIndex = addQueue(SERVICE_QUEUE_NAME, SERVICE_QUEUE_LENGTH, SERVICE_QUEUE_SIZE);

        // create control queue
        controlQueueIndex = addQueue(getControlQueueName(), CONTROL_QUEUE_LENGTH, sizeof(std::uint8_t));

        // create and add all queues provided from service
        for (const auto &wqi : queuesList) {
            addQueue(wqi.name, wqi.length, wqi.elementSize);
        };

        // iterate over all user queues and add them to set
        for (std::uint32_t i = 0; i < queues.size(); ++i) {
            if (xQueueAddToSet(queues[i]->GetQueueHandle(), queueSet) != pdPASS) {
                state = State::Invalid;
                deinit();
                return false;
            }
        }

        // create join semaphore
        joinSemaphore = xSemaphoreCreateBinary();

        // state protector
        stateMutex = xSemaphoreCreateMutex();

        // it is safe to use getState/setState methods now
        setState(State::Initiated);

        return true;
    }

    bool Worker::deinit()
    {
        // for all queues - remove from set and delete queue
        for (auto &q : queues) {
            // remove queues from set
            xQueueRemoveFromSet(q->GetQueueHandle(), queueSet);
        }
        queues.clear();

        // delete queues set
        vQueueDelete(static_cast<QueueHandle_t>(queueSet));

        vSemaphoreDelete(joinSemaphore);
        vSemaphoreDelete(stateMutex);

        setState(State::Destroyed);

        return true;
    };

    /**
     * This method starts RTOS thread that waits for incoming queue events.
     */
    bool Worker::run()
    {
        assert(getState() == State::Initiated);

        BaseType_t task_error = 0;
        task_error            = xTaskCreate(
            Worker::taskAdapter, name.c_str(), stackDepth / sizeof(StackType_t), this, priority, &taskHandle);

        if (task_error != pdPASS) {
            LOG_ERROR("Failed to start the task");
            return false;
        }

        return true;
    }

    bool Worker::stop()
    {
        assert(getState() == State::Running);
        return sendControlMessage(ControlMessage::Stop);
    }

    bool Worker::sendControlMessage(ControlMessage message)
    {
        auto messageToSend = static_cast<std::uint8_t>(message);
        return getControlQueue().Enqueue(&messageToSend, portMAX_DELAY);
    }

    bool Worker::sendCommand(WorkerCommand command)
    {
        return getServiceQueue().Enqueue(&command);
    }

    bool Worker::send(std::uint32_t cmd, std::uint32_t *data)
    {
        assert(getState() == State::Running);

        WorkerCommand workerCommand{cmd, data};
        return getServiceQueue().Enqueue(&workerCommand, portMAX_DELAY);
    }

    xQueueHandle Worker::getQueueHandleByName(const std::string &qname) const
    {
        for (auto &q : queues) {
            if (q->GetQueueName() == qname) {
                return q->GetQueueHandle();
            }
        }
        return nullptr;
    }

    std::shared_ptr<WorkerQueue> Worker::getQueueByName(const std::string &qname) const
    {
        for (auto &q : queues) {
            if (q->GetQueueName() == qname) {
                return q;
            }
        }
        return nullptr;
    }

    bool Worker::join(TickType_t timeout)
    {
        assert(getState() == State::Running || getState() == State::Stopped);
        if (xSemaphoreTake(joinSemaphore, timeout) != pdTRUE) {
            LOG_ERROR("Failed to take semaphore!");
            return false;
        }
        const auto waitDeleteTaskTick = xTaskGetTickCount();
        while (eTaskGetState(taskHandle) != eDeleted) {
            if ((xTaskGetTickCount() - waitDeleteTaskTick) > timeout) {
                LOG_ERROR("Task waiting aborted (timeout). TaskState != eDeleted");
                return false;
            }
        }
        return true;
    }

    void Worker::setState(State newState)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state = newState;
        xSemaphoreGive(stateMutex);
    }

    Worker::State Worker::getState() const
    {
        State currentState;

        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState = state;
        xSemaphoreGive(stateMutex);

        return currentState;
    }

    void Worker::close()
    {
        if (!stop() || !join()) { // timeout join
            kill();
        }
        deinit();
    }

    void Worker::kill()
    {
        // do not check state - this is intentional, we want to be able to kill
        // a worker in case of unexpected failure without knowing its state.
        vTaskDelete(taskHandle);
    }
} /* namespace sys */

#include "Worker.hpp"

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

#include <map>
#include <string>
#include <utility>

#include <cassert>

namespace sys
{
    unsigned int Worker::count = 0;

    void Worker::taskAdapter(void *taskParam)
    {
        Worker *worker = static_cast<Worker *>(taskParam);
        worker->task();
    }

    bool Worker::handleControlMessage()
    {
        std::uint8_t receivedMessage;

        xQueueReceive(controlQueue, &receivedMessage, 0);
        LOG_INFO("Handle control message: %u", receivedMessage);
        assert(receivedMessage < controlMessagesCount);

        switch (static_cast<Worker::ControlMessage>(receivedMessage)) {
        // stop the thread
        case ControlMessage::Stop: {
            setState(State::Stopping);
        } break;

        default: {
            LOG_FATAL("Unexpected control message %d received", receivedMessage);
            return false;
        } break;
        }

        return true;
    }

    void Worker::task()
    {
        QueueSetMemberHandle_t activeMember;

        while (getState() == State::Running) {
            activeMember = xQueueSelectFromSet(queueSet, portMAX_DELAY);

            // handle control messages from parent service
            if (activeMember == controlQueue) {
                handleControlMessage();
                continue;
            }

            // find id of the queue that was activated
            for (uint32_t i = 0; i < queues.size(); i++) {
                if (queues[i] == activeMember) {
                    handleMessage(i);
                }
            }
        }

        // inform about thread end and wait for the deletion
        xSemaphoreGive(joinSemaphore);
        setState(State::Stopped);
        vTaskDelete(nullptr);
    }

    Worker::Worker(sys::Service *service) : service{service}
    {}

    Worker::~Worker()
    {
        if (state != State::Destroyed) {
            LOG_FATAL("Calling destructor of an undestroyed worker.");
        }
    }

    void Worker::addQueueInfo(xQueueHandle q, std::string qName)
    {
        queueNameMap.emplace(std::make_pair(q, qName));
        vQueueAddToRegistry(q, qName.c_str());
        queues.push_back(q);
    }

    inline std::string Worker::getControlQueueName() const
    {
        return controlQueueNamePrefix + std::to_string(id);
    }

    bool Worker::init(std::list<WorkerQueueInfo> queuesList)
    {
        assert(state == State::New);

        // assign worker id
        taskENTER_CRITICAL();
        id = count++;
        taskEXIT_CRITICAL();

        name = service->GetName() + "_w" + std::to_string(id);

        // initial value is because there is always a service and control queue
        // to communicate with the parent service
        auto setSize = SERVICE_QUEUE_LENGTH + CONTROL_QUEUE_LENGTH;

        // iterate over all entries in the list of queues and summarize queue sizes
        for (auto wqi : queuesList) {
            setSize += wqi.length;
        }

        // create set of queues
        queueSet = xQueueCreateSet(setSize);
        if (queueSet == nullptr) {
            state = State::Invalid;
            return false;
        }

        // create and add all queues to the set. First service queue is created.
        serviceQueue = xQueueCreate(SERVICE_QUEUE_LENGTH, SERVICE_QUEUE_SIZE);
        if (serviceQueue == nullptr) {
            state = State::Invalid;
            deinit();
            return false;
        }

        addQueueInfo(serviceQueue, SERVICE_QUEUE_NAME);

        // create control queue
        controlQueue = xQueueCreate(CONTROL_QUEUE_LENGTH, sizeof(std::uint8_t));
        if (controlQueue == nullptr) {
            state = State::Invalid;
            deinit();
            return false;
        }

        addQueueInfo(controlQueue, getControlQueueName());

        // create and add all queues provided from service
        for (auto wqi : queuesList) {
            auto q = xQueueCreate(wqi.length, wqi.elementSize);
            if (q == nullptr) {
                LOG_FATAL("xQueueCreate %s failed", wqi.name.c_str());
                state = State::Invalid;
                deinit();
                return false;
            }
            addQueueInfo(q, wqi.name);
        };

        // iterate over all queues and add them to set
        for (uint32_t i = 0; i < queues.size(); ++i) {

            if (xQueueAddToSet(queues[i], queueSet) != pdPASS) {
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
        for (auto q : queues) {
            // remove queues from set
            xQueueRemoveFromSet(q, queueSet);
            // delete queue
            vQueueDelete(q);
        }
        queues.clear();

        // delete queues set
        vQueueDelete((QueueHandle_t)queueSet);

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

        runnerTask = xTaskGetCurrentTaskHandle();

        BaseType_t task_error =
            xTaskCreate(Worker::taskAdapter, name.c_str(), defaultStackSize, this, service->GetPriority(), &taskHandle);
        if (task_error != pdPASS) {
            LOG_ERROR("Failed to start the task");
            return false;
        }

        setState(State::Running);
        return true;
    }

    bool Worker::stop()
    {
        assert(xTaskGetCurrentTaskHandle() == runnerTask);
        assert(getState() == State::Running);

        return sendControlMessage(ControlMessage::Stop);
    }

    bool Worker::sendControlMessage(ControlMessage message)
    {
        auto messageToSend = static_cast<std::uint8_t>(message);
        return xQueueSend(controlQueue, &messageToSend, portMAX_DELAY) == pdTRUE;
    }

    bool Worker::send(uint32_t cmd, uint32_t *data)
    {
        assert(xTaskGetCurrentTaskHandle() == runnerTask);
        assert(getState() == State::Running);

        if (serviceQueue != nullptr) {
            WorkerCommand workerCommand{cmd, data};
            if (xQueueSend(serviceQueue, &workerCommand, portMAX_DELAY) == pdTRUE) {
                return true;
            }
        }
        return false;
    }

    xQueueHandle Worker::getQueueByName(std::string qname)
    {
        for (auto q_handle : this->queues) {
            if (this->queueNameMap[q_handle] == qname)
                return q_handle;
        }
        return nullptr;
    }

    bool Worker::join(TickType_t timeout)
    {
        assert(xTaskGetCurrentTaskHandle() == runnerTask);
        assert(getState() == State::Running);

        if (xSemaphoreTake(joinSemaphore, timeout) != pdTRUE) {
            return false;
        }
        while (eTaskGetState(taskHandle) != eDeleted) {}

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
        if (!stop() || !join()) {
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

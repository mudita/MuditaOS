// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service.hpp"

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <queue.hpp>

namespace sys
{

    class WorkerQueueInfo
    {
      public:
        WorkerQueueInfo(std::string_view _name, int _elementSize, int _length)
            : name(_name), elementSize(_elementSize), length(_length)
        {}
        std::string name;
        int elementSize;
        int length;
    };

    class WorkerQueue : public cpp_freertos::Queue
    {
      public:
        WorkerQueue(const std::string &name, UBaseType_t maxItems, UBaseType_t itemSize)
            : Queue(maxItems, itemSize), name(name)
        {}

        QueueHandle_t GetQueueHandle() const
        {
            return handle;
        }

        const std::string &GetQueueName() const
        {
            return name;
        }

      private:
        const std::string name;
    };

    struct WorkerCommand
    {
        uint32_t command = 0;
        uint32_t *data   = NULL;
    };

    /*
     * @brief Worker is a wrapper for freeRTOS task used to separate sysmanager environment from
     * the rest of the system. Its purpose is to handle asynchronous events like IRQ or timers.
     * Flow of creating worker is as follows:
     * - create new Worker object and provide pointer to the service that owns the worker,
     * - call init method and provide list of parameters to create queues. Those queues can be later
     * used to wake up the worker.
     * - call run method to start the worker.
     *
     * Flow for closing the worker is as follows:
     * - call stop method - task will end itself
     * - call join method to wait for the task to end
     * - call deinit to destroy all resources owned by the worker
     * - delete the object.
     *
     */
    class Worker
    {
      private:
        enum class ControlMessage
        {
            Stop,
            MessageCount
        };

        enum class State
        {
            New,
            Initiated,
            Running,
            Stopping,
            Stopped,
            Destroyed,
            Invalid
        };

        using Id = unsigned int;

        static void taskAdapter(void *taskParam);
        bool handleControlMessage();
        void task();
        void setState(State newState);
        void constructName();
        std::string getControlQueueName() const;
        size_t addQueue(const std::string &queueName, UBaseType_t maxItems, UBaseType_t itemSize);

        void kill();
        bool join(TickType_t timeout = defaultTimeout);

        std::optional<size_t> controlQueueIndex;
        std::optional<size_t> serviceQueueIndex;
        WorkerQueue &getControlQueue() const;

        static constexpr std::size_t controlMessagesCount = static_cast<std::size_t>(ControlMessage::MessageCount);
        static constexpr std::size_t defaultStackSize     = 8192;
        static constexpr TickType_t defaultTimeout        = pdMS_TO_TICKS(5000);
        static constexpr auto controlQueueNamePrefix      = "wctrl";

        xSemaphoreHandle joinSemaphore = nullptr;
        xTaskHandle runnerTask         = nullptr;
        xSemaphoreHandle stateMutex    = nullptr;
        xTaskHandle taskHandle         = nullptr;

        Id id;
        std::string name;
        State state = State::New;

      protected:
        virtual bool handleMessage(uint32_t queueID) = 0;

        WorkerQueue &getServiceQueue() const;

        xQueueHandle getQueueHandleByName(const std::string &qname) const;
        std::shared_ptr<WorkerQueue> getQueueByName(const std::string &qname) const;

        bool sendControlMessage(ControlMessage message);
        bool sendCommand(WorkerCommand command);
        State getState() const;

        const static uint32_t SERVICE_QUEUE_LENGTH = 10;
        const static uint32_t CONTROL_QUEUE_LENGTH = 4;
        const static uint32_t SERVICE_QUEUE_SIZE   = sizeof(WorkerCommand);
        const std::string SERVICE_QUEUE_NAME       = "ServiceQueue";

        static unsigned int count;
        const UBaseType_t priority;
        std::uint16_t stackDepth = defaultStackSize;

        QueueSetHandle_t queueSet = nullptr;
        std::vector<std::shared_ptr<WorkerQueue>> queues;

      public:
        explicit Worker(sys::Service *service, std::uint16_t stackDepth = defaultStackSize);
        Worker(std::string workerNamePrefix, const UBaseType_t priority, std::uint16_t stackDepth = defaultStackSize);

        virtual ~Worker();

        /**
         * @brief This function is responsible for creating all queues provided in the constructor.
         * When all queues are created this method creates set of queues.
         */
        virtual bool init(std::list<WorkerQueueInfo> queuesList = std::list<WorkerQueueInfo>());
        /**
         * @brief This function is responsible for destroying all resources created in the
         * init mehtod.
         */
        virtual bool deinit();
        /**
         * @brief Starts RTOS thread that waits for incoming queue events.
         */
        virtual bool run();
        /**
         * @brief Sends stop command to worker.
         */
        virtual bool stop();

        /**
         * @brief Closes worker by combining stop, join and deinit operations in a single call.
         * If it is not possible to close the worker gently it would kill it forcibly.
         */
        void close(TickType_t timeout = defaultTimeout);
    };
} /* namespace sys */

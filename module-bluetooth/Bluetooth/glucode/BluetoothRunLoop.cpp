// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothRunLoop.hpp"
#include "btstack_util.h"
#include <cassert>
namespace bluetooth
{

    btstack_linked_list_t RunLoop::timers;
    btstack_linked_list_t RunLoop::data_sources;
    bool RunLoop::run_loop_exit_requested;

    QueueHandle_t RunLoop::btstack_run_loop_queue;
    TaskHandle_t RunLoop::btstack_run_loop_task;
    QueueHandle_t RunLoop::triggerQueue;
    TimerHandle_t RunLoop::testTimer = nullptr;

    auto RunLoop::removeTimer(btstack_timer_source_t *ts) -> bool
    {
        return btstack_linked_list_remove(&timers, reinterpret_cast<btstack_linked_item_t *>(ts));
    }
    void RunLoop::setTriggerQueue(QueueHandle_t queue)
    {
        assert(queue != nullptr);
        triggerQueue = queue;
    }
    void RunLoop::deinit()
    {
        vQueueDelete(btstack_run_loop_queue);
        xTimerDelete(testTimer, 0);
    }
    void RunLoop::init()
    {
        timers                 = nullptr;
        btstack_run_loop_queue = xQueueCreate(RUN_LOOP_QUEUE_LENGTH, RUN_LOOP_QUEUE_ITEM_SIZE);

        // task to handle to optimize 'run on main thread'
        btstack_run_loop_task = xTaskGetCurrentTaskHandle();

        LOG_INFO("run loop init, task %p, queue item size %u",
                 btstack_run_loop_task,
                 static_cast<int>(sizeof(function_call_t)));
    }
    void RunLoop::enableDataSourceCallbacks(btstack_data_source_t *ds, uint16_t callback_types)
    {
        ds->flags |= callback_types;
    }

    void RunLoop::disableDataSourceCallbacks(btstack_data_source_t *ds, uint16_t callback_types)
    {
        ds->flags &= ~callback_types;
    }

    void RunLoop::addDataSource(btstack_data_source_t *ds)
    {
        btstack_linked_list_add(&data_sources, (btstack_linked_item_t *)ds);
    }

    auto RunLoop::removeDataSource(btstack_data_source_t *ds) -> bool
    {
        return btstack_linked_list_remove(&data_sources, (btstack_linked_item_t *)ds);
    }
    void RunLoop::triggerExit()
    {
        run_loop_exit_requested = true;
    }
    auto RunLoop::getTimeMs() -> TickType_t
    {
        return xTaskGetTickCount();
    }
    void RunLoop::trigger()
    {

        bool trigger = true;
        if (triggerQueue != nullptr) {
            xQueueSend(triggerQueue, &trigger, 0);
        }
        else {
            LOG_FATAL("Trigger queue does not exist!");
        }
    }
    void RunLoop::executeCodeOnMainThread(void (*fn)(void *arg), void *arg)
    {

        // directly call function if already on btstack task
        if (xTaskGetCurrentTaskHandle() == btstack_run_loop_task) {
            (*fn)(arg);
            return;
        }

        function_call_t message;
        message.fn     = fn;
        message.arg    = arg;
        BaseType_t res = xQueueSendToBack(btstack_run_loop_queue, &message, 0); // portMAX_DELAY);
        if (res != pdTRUE) {
            LOG_ERROR("Failed to post fn %p", fn);
        }
        trigger();
    }

    void RunLoop::addTimer(btstack_timer_source_t *ts)
    {
        btstack_linked_item_t *it = nullptr;
        for (it = reinterpret_cast<btstack_linked_item_t *>(&timers); it->next != nullptr; it = it->next) {
            // don't add timer that's already in there
            auto *next = reinterpret_cast<btstack_timer_source_t *>(it->next);
            if (next == ts) {
                LOG_ERROR("btstack_run_loop_timer_add error: timer to add already in list!");
                return;
            }
            // exit if new timeout before list timeout
            int32_t delta = btstack_time_delta(ts->timeout, next->timeout);
            if (delta < 0) {
                break;
            }
        }
        ts->item.next = it->next;
        it->next      = reinterpret_cast<btstack_linked_item_t *>(ts);
        trigger();
    }
    void RunLoop::setTimer(btstack_timer_source_t *ts, uint32_t timeout_in_ms)
    {
        ts->timeout = getTimeMs() + timeout_in_ms + 1;
        trigger();
    }
    void RunLoop::triggerCallback(TimerHandle_t xTimer)
    {
        trigger();
    }
    void RunLoop::start()
    {
        if (testTimer == nullptr) {
            testTimer = xTimerCreate("TestTimer", pdMS_TO_TICKS(1000), pdTRUE, nullptr, triggerCallback);
            xTimerStart(testTimer, 0);
        }
    }
    auto RunLoop::process() -> bool
    {
        // process registered function calls on run loop thread
        while (true) {
            function_call_t message = {nullptr, nullptr};
            BaseType_t res          = xQueueReceive(btstack_run_loop_queue, &message, 0);
            if (res == pdFALSE) {
                break;
            }
            if (message.fn != nullptr) {
                message.fn(message.arg);
            }
        }

        // process timers and get next timeout
        uint32_t timeout_ms = 1000;
        while (timers != nullptr) {
            auto *ts         = reinterpret_cast<btstack_timer_source_t *>(timers);
            uint32_t now     = getTimeMs();
            int32_t delta_ms = btstack_time_delta(ts->timeout, now);
            if (delta_ms > 0) {
                timeout_ms = delta_ms;
                break;
            }
            // remove timer before processing it to allow handler to re-register with run loop
            removeTimer(ts);
            ts->process(ts);
        }

        // exit triggered by btstack_run_loop_freertos_trigger_exit (from data source, timer, run on main thread)
        if (run_loop_exit_requested) {
            return true;
        }

        xTimerChangePeriod(testTimer, pdMS_TO_TICKS(timeout_ms), 0);
        return false;
    }

    auto RunLoop::getRunLoopInstance() -> btstack_run_loop *
    {

        runLoop = btstack_run_loop{
            &init,
            &addDataSource,
            &removeDataSource,
            &enableDataSourceCallbacks,
            &disableDataSourceCallbacks,
            &setTimer,
            &addTimer,
            &removeTimer,
            &start,
            nullptr,
            &getTimeMs,
        };
        return &runLoop;
    }

} // namespace bluetooth

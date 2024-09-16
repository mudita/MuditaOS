// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BluetoothRunLoop.hpp"
#include "btstack_util.h"
#include <log/log.hpp>
#include <cassert>

namespace bluetooth
{
    QueueHandle_t RunLoop::btstackRunLoopQueue;
    TaskHandle_t RunLoop::btstackRunLoopTask;
    QueueHandle_t RunLoop::triggerQueue;
    TimerHandle_t RunLoop::btstackTimer = nullptr;
    btstack_linked_list_t RunLoop::timers;
    btstack_linked_list_t RunLoop::dataSources;
    bool RunLoop::runLoopExitRequested;

    bool RunLoop::removeTimer(btstack_timer_source_t *timerSource)
    {
        return btstack_linked_list_remove(&timers, reinterpret_cast<btstack_linked_item_t *>(timerSource));
    }

    void RunLoop::setTriggerQueue(QueueHandle_t queue)
    {
        assert(queue != nullptr);
        triggerQueue = queue;
    }

    void RunLoop::deinit()
    {
        vQueueDelete(btstackRunLoopQueue);
        xTimerDelete(btstackTimer, 0);
    }

    void RunLoop::init()
    {
        timers                 = nullptr;
        btstackRunLoopQueue    = xQueueCreate(runLoopQueueLength, runLoopQueueItemSize);

        // Task to handle to optimize 'run on main thread'
        btstackRunLoopTask = xTaskGetCurrentTaskHandle();

        LOG_INFO("Run loop init, task %p, queue item size %zu", btstackRunLoopTask, sizeof(FunctionCallType));
    }

    void RunLoop::enableDataSourceCallbacks(btstack_data_source_t *dataSource, std::uint16_t callbackTypes)
    {
        dataSource->flags |= callbackTypes;
    }

    void RunLoop::disableDataSourceCallbacks(btstack_data_source_t *dataSource, std::uint16_t callbackTypes)
    {
        dataSource->flags &= ~callbackTypes;
    }

    void RunLoop::addDataSource(btstack_data_source_t *dataSource)
    {
        btstack_linked_list_add(&dataSources, reinterpret_cast<btstack_linked_item_t *>(dataSource));
    }

    auto RunLoop::removeDataSource(btstack_data_source_t *dataSource) -> bool
    {
        return btstack_linked_list_remove(&dataSources, reinterpret_cast<btstack_linked_item_t *>(dataSource));
    }

    void RunLoop::triggerExit()
    {
        runLoopExitRequested = true;
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

    void RunLoop::addTimer(btstack_timer_source_t *timerSource)
    {
        btstack_linked_item_t *it = nullptr;
        for (it = reinterpret_cast<btstack_linked_item_t *>(&timers); it->next != nullptr; it = it->next) {
            // Don't add timer that's already in there
            auto next = reinterpret_cast<btstack_timer_source_t *>(it->next);
            if (next == timerSource) {
                LOG_ERROR("Timer 'btstack_run_loop_timer' already in list!");
                return;
            }

            // Exit if new timeout before list timeout
            const auto delta = btstack_time_delta(timerSource->timeout, next->timeout);
            if (delta < 0) {
                break;
            }
        }
        timerSource->item.next = it->next;
        it->next               = reinterpret_cast<btstack_linked_item_t *>(timerSource);
        trigger();
    }

    void RunLoop::setTimer(btstack_timer_source_t *timerSource, std::uint32_t timeoutMs)
    {
        timerSource->timeout = getTimeMs() + timeoutMs + 1;
        trigger();
    }

    void RunLoop::triggerCallback([[maybe_unused]] TimerHandle_t xTimer)
    {
        trigger();
    }

    void RunLoop::start()
    {
        if (btstackTimer == nullptr) {
            btstackTimer =
                xTimerCreate("BTStackTimer", pdMS_TO_TICKS(defaultTimerPeriodMs), pdTRUE, nullptr, triggerCallback);
            xTimerStart(btstackTimer, 0);
        }
    }

    auto RunLoop::process() -> bool
    {
        // Process registered function calls on run loop thread
        while (true) {
            FunctionCallType message = {nullptr, nullptr};
            const auto res           = xQueueReceive(btstackRunLoopQueue, &message, 0);
            if (res == pdFALSE) {
                break;
            }
            if (message.fn != nullptr) {
                message.fn(message.arg);
            }
        }

        // Process timers and get next timeout
        std::uint32_t timeoutMs = defaultTimerPeriodMs;
        while (timers != nullptr) {
            auto timerSource   = reinterpret_cast<btstack_timer_source_t *>(timers);
            const auto now     = getTimeMs();
            const auto deltaMs = btstack_time_delta(timerSource->timeout, now);
            if (deltaMs > 0) {
                timeoutMs = deltaMs;
                break;
            }

            // Remove timer before processing it to allow handler to re-register with run loop
            removeTimer(timerSource);
            timerSource->process(timerSource);
        }

        // Exit triggered by btstack_run_loop_freertos_trigger_exit (from data source, timer, run on main thread)
        if (runLoopExitRequested) {
            return true;
        }

        xTimerChangePeriod(btstackTimer, pdMS_TO_TICKS(timeoutMs), 0);
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

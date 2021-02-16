// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <log/log.hpp>
#include "queue.hpp"
#include <btstack_run_loop.h>
#include <timer.hpp>

namespace bluetooth
{

    using function_call_t = struct function_call
    {
        void (*fn)(void *arg);
        void *arg;
    };

    constexpr inline auto RUN_LOOP_QUEUE_LENGTH    = 20;
    constexpr inline auto RUN_LOOP_QUEUE_ITEM_SIZE = sizeof(function_call_t);

    class RunLoop
    {
      private:
        static auto removeTimer(btstack_timer_source_t *ts) -> bool;
        static void init();
        static void enableDataSourceCallbacks(btstack_data_source_t *ds, uint16_t callback_types);
        static void disableDataSourceCallbacks(btstack_data_source_t *ds, uint16_t callback_types);
        static void addDataSource(btstack_data_source_t *ds);
        static auto removeDataSource(btstack_data_source_t *ds) -> bool;
        static void triggerExit();
        static auto getTimeMs() -> TickType_t;
        static void executeCodeOnMainThread(void (*fn)(void *arg), void *arg);
        static void addTimer(btstack_timer_source_t *ts);
        static void setTimer(btstack_timer_source_t *ts, uint32_t timeout_in_ms);
        static void start();
        static void triggerCallback(TimerHandle_t xTimer);
        static QueueHandle_t btstack_run_loop_queue;
        static TaskHandle_t btstack_run_loop_task;
        static QueueHandle_t triggerQueue;
        static TimerHandle_t testTimer;
        static btstack_linked_list_t timers;
        static btstack_linked_list_t data_sources;
        static bool run_loop_exit_requested;
        btstack_run_loop runLoop;

      public:
        static void trigger();
        auto process() -> bool;
        static void deinit();
        void setTriggerQueue(QueueHandle_t queue);
        auto getRunLoopInstance() -> btstack_run_loop *;
    };

} // namespace bluetooth

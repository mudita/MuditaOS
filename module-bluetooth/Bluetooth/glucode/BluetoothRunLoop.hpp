// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <btstack_run_loop.h>
#include <queue.hpp>
#include <timer.hpp>

namespace bluetooth
{
    using FunctionCallType = struct FunctionCall
    {
        void (*fn)(void *arg);
        void *arg;
    };

    inline constexpr auto runLoopQueueLength   = 20;
    inline constexpr auto runLoopQueueItemSize = sizeof(FunctionCallType);

    class RunLoop
    {
      public:
        static void trigger();
        static bool process();
        static void deinit();
        static void setTriggerQueue(QueueHandle_t queue);
        auto getRunLoopInstance() -> btstack_run_loop *;

      private:
        static bool removeTimer(btstack_timer_source_t *timerSource);

        static void init();

        static void enableDataSourceCallbacks(btstack_data_source_t *dataSource, std::uint16_t callbackTypes);
        static void disableDataSourceCallbacks(btstack_data_source_t *dataSource, std::uint16_t callbackTypes);

        static void addDataSource(btstack_data_source_t *dataSource);
        static bool removeDataSource(btstack_data_source_t *dataSource);

        static void triggerExit();

        static auto getTimeMs() -> TickType_t;

        static void addTimer(btstack_timer_source_t *ts);
        static void setTimer(btstack_timer_source_t *ts, uint32_t timeoutMs);

        static void start();
        static void triggerCallback(TimerHandle_t xTimer);

        static constexpr auto defaultTimerPeriodMs = 1000;

        static QueueHandle_t btstackRunLoopQueue;
        static TaskHandle_t btstackRunLoopTask;
        static QueueHandle_t triggerQueue;
        static TimerHandle_t btstackTimer;
        static btstack_linked_list_t timers;
        static btstack_linked_list_t dataSources;
        static bool runLoopExitRequested;

        btstack_run_loop runLoop;
    };
} // namespace bluetooth

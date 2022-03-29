// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <queue.h>
#include <functional>

namespace bsp
{
    namespace pit
    {
        enum class Event
        {
            Overflow
        };
        void init(xQueueHandle qhandle);
        void start(uint32_t usec, std::function<void(void)> iqr_cb);
        void stop();
    }; // namespace pit
};     // namespace bsp

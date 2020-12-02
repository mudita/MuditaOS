// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

namespace bsp::light_sensor
{
    std::int32_t init(xQueueHandle qHandle);

    void deinit();

    bool standby();

    void wakeup();

    bool reset();

    bool isPresent();

    void readout();

    BaseType_t IRQHandler();
} // namespace bsp::light_sensor

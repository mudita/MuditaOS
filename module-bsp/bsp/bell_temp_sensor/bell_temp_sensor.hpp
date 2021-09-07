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

namespace bsp::bell_temp_sensor
{
    using Temperature = float;

    std::int32_t init(bool Fahrenheit = false);

    void deinit();

    bool standby();

    bool wakeup();

    bool isPresent();

    Temperature readout();

} // namespace bsp::bell_temp_sensor

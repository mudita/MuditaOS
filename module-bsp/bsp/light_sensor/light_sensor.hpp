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
    using IlluminanceLux = float;

    std::int32_t init();

    void deinit();

    bool standby();

    bool wakeup();

    bool reset();

    bool isPresent();

    IlluminanceLux readout();
} // namespace bsp::light_sensor

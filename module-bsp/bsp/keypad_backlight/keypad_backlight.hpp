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

namespace bsp::keypad_backlight
{
    enum class Action
    {
        turnOn,
        turnOff,
        checkState,
    };

    using DiodeIntensity = float;

    std::int32_t init();

    void deinit();

    bool turnOnAll();

    bool configureModule();

    bool shutdown();

    void wakeup();

    bool reset();

    bool checkState();

} // namespace bsp::keypad_backlight

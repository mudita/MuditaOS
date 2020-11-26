// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

namespace bsp
{
    namespace keypad_backlight
    {
        enum class Action
        {
            turnOn,
            turnOff,
            checkState,
        };

        // channel intensity 0.0 - 1.0 : min - max
        using DiodeIntensity = float;

        int32_t init();

        void deinit();

        // Turn on whole backlight
        bool turnOnAll();

        // Configure mode of operation and enable output
        bool configureModule();

        // Module shutdown - registers erased
        bool shutdown();

        // Wakeup module - registers in default state
        void wakeup();

        // Module reset thorugh I2C interface
        bool reset();

        // Check if backlight is on
        bool checkState();

    } // namespace keypad_backlight
} // namespace bsp

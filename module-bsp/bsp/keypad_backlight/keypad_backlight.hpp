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
        };

        // channel intensity 0.0 - 1.0 : min - max
        using DiodeIntensity = float;

        struct Rgb
        {
            DiodeIntensity red;
            DiodeIntensity green;
            DiodeIntensity blue;
        };

        enum class Diodes
        {
            RGB_LEFT,
            RGB_RIGHT,
            KEYPAD_LEFT,
            KEYPAD_RIGHT,
        };

        int32_t init(xQueueHandle qHandle);

        void deinit();

        // Turn on whole backlight
        bool turnOnAll();

        // Single diode
        bool set(Diodes diode, DiodeIntensity intensity);

        // RGB diode
        bool set(Diodes diode, Rgb intensity);

        // Configure mode of operation and enable output
        bool configureModule();

        // Module shutdown - registers erased
        void shutdown();

        // Wakeup module - registers in default state
        void wakeup();

        // Module reset thorugh I2C interface
        bool reset();

    } // namespace keypad_backlight
} // namespace bsp

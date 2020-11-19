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
    namespace backlight
    {
        // channel intensity 0.0 - 1.0 : min - max
        using DiodeIntensity = float;

        struct Rgb
        {
            DiodeIntensity red;
            DiodeIntensity green;
            DiodeIntensity blue;
        };

        enum class Diode
        {
            RGB_LEFT,
            RGB_RIGHT,
            KEYPAD_BACKLIGHT_1,
            KEYPAD_BACKLIGHT_2,
            KEYPAD_ALL,
        };

        int32_t init(xQueueHandle qHandle);

        void deinit();

        // Single diode
        bool set(Diode diode, DiodeIntensity intensity);

        // RGB diode
        bool set(Diode diode, Rgb intensity);
        
        // Toggle diode, diode intensity preserved
        bool toggle(Diode diode);

        // Module sleep mode
        void backlight_shutdown();

        // Wakeup module, last on intensity preserved
        void backlight_wakeup();

        // Module reset
        void backlight_reset();

    } // namespace backlight
} // namespace bsp

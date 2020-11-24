// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/keypad_backlight/keypad_backlight.hpp"
#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

namespace bsp
{

    namespace keypad_backlight
    {
        static xQueueHandle qHandleIrq = NULL;

        int32_t init(xQueueHandle qHandle)
        {
            qHandleIrq = qHandle;

            return 0;
        }

        void deinit()
        {
            qHandleIrq = NULL;
        }

        bool set(Diodes diode, DiodeIntensity intensity)
        {
            return true;
        }

        bool set(Diodes diode, Rgb intensity)
        {
            return true;
        }

        bool turnOnAll()
        {
            return true;
        }

        bool configureModule()
        {
            return true;
        }

        bool shutdown()
        {
            return true;
        }

        void wakeup()
        {}

        bool reset()
        {
            return true;
        }
    } // namespace keypad_backlight
} // namespace bsp

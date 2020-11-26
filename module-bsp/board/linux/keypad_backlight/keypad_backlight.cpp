// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/keypad_backlight/keypad_backlight.hpp"
#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

namespace bsp
{
    namespace keypad_backlight
    {
        bool diodes_state = false;

        int32_t init()
        {
            return 0;
        }

        void deinit()
        {}

        bool turnOnAll()
        {
            diodes_state = true;
            return true;
        }

        bool configureModule()
        {
            return true;
        }

        bool shutdown()
        {
            diodes_state = false;
            return true;
        }

        void wakeup()
        {}

        bool reset()
        {
            return true;
        }

        bool checkState()
        {
            return diodes_state;
        }

    } // namespace keypad_backlight
} // namespace bsp

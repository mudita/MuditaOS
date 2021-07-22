// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bell_switches/bell_switches.hpp"


namespace bsp
{
    namespace bell_switches
    {
        int32_t init(xQueueHandle qHandle)
        {
            return 1;
        }
    } // namespace bell_switches
} // namespace bsp

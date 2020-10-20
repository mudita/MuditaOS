// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "headset.hpp"

namespace bsp
{
    namespace headset
    {

        int32_t Init(xQueueHandle qHandle)
        {
            return 1;
        }

        bool Handler(uint8_t notification)
        {
            return false;
        }

        bool IsInserted()
        {
            return false;
        }

        int32_t Deinit()
        {
            return 1;
        }
    } // namespace headset
} // namespace bsp

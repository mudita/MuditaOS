// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

        HeadsetState headset_get_data(bool &headsetState, bool &microphoneState, uint8_t &keyEvent, uint8_t &keyCode)
        {
            return HeadsetState::NoChange;
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

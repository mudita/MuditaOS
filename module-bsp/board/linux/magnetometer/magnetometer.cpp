// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/magnetometer/magnetometer.hpp"

#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace magnetometer
    {

        int32_t init(xQueueHandle qHandle)
        {

            qHandleIrq = qHandle;
            return 1;
        }

        bool isPresent(void)
        {
            return false;
        }

        bsp::Board GetBoard(void)
        {
            return bsp::Board::Linux;
        }
    } // namespace magnetometer
} // namespace bsp

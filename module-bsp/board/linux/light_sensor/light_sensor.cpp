// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/light_sensor/light_sensor.hpp"

namespace bsp::light_sensor
{
    namespace
    {
        xQueueHandle qHandleIrq = nullptr;
    } // namespace

    std::int32_t init(xQueueHandle qHandle)
    {
        qHandleIrq = qHandle;
        return 1;
    }

    void deinit()
    {
        qHandleIrq = nullptr;
    }

    bool standby()
    {
        return true;
    }

    bool wakeup()
    {
        return true;
    }

    bool reset()
    {
        return true;
    }

    bool isPresent()
    {
        return true;
    }

    IlluminanceLux readout()
    {
        return 0.0f;
    }

    BaseType_t IRQHandler()
    {
        return pdFALSE;
    }
} // namespace bsp::light_sensor

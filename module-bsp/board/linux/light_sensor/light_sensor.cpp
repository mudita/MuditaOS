// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/light_sensor/light_sensor.hpp"

namespace bsp::light_sensor
{

    std::int32_t init()
    {
        return 1;
    }

    void deinit()
    {}

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
} // namespace bsp::light_sensor

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bell_temp_sensor/bell_temp_sensor.hpp"
#include <log.hpp>

namespace bsp::bell_temp_sensor
{
    bool isFahrenheit = false;

    std::int32_t init(bool Fahrenheit)
    {
        isFahrenheit = Fahrenheit;

        return 0;
    }

    void deinit()
    {
        
    }

    bool standby()
    {
        return true;
    }

    bool wakeup()
    {
        return true;
    }

    Temperature readout()
    {
        float temp = 21.0;
    
        if (isFahrenheit)
            temp = (temp * 1.8) + 32.00;

        return temp;
    }

    bool isPresent()
    {
        return true;
    }

} // namespace bsp::bell_temp_sensor

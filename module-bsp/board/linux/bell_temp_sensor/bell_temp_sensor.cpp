// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bell_temp_sensor/bell_temp_sensor.hpp"
#include <log.hpp>

namespace bsp::bell_temp_sensor
{
    bool isFahrenheit                          = false;
    constexpr auto celsius_temperature_mock    = 21.0;
    constexpr auto fahrenheit_temperature_mock = (celsius_temperature_mock * 1.8) + 32;

    std::int32_t init(bool Fahrenheit)
    {
        isFahrenheit = Fahrenheit;

        return 0;
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

    Temperature readout()
    {
        if (isFahrenheit)
            return fahrenheit_temperature_mock;
        else
            return celsius_temperature_mock;
    }

    bool isPresent()
    {
        return true;
    }

} // namespace bsp::bell_temp_sensor

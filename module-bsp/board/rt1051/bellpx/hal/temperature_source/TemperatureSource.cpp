// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <hal/temperature_source/TemperatureSource.hpp>
#include <hal/GenericFactory.hpp>
#include "bsp/bell_temp_sensor/bell_temp_sensor.hpp"

namespace hal::temperature
{
    class BellTemperatureSource : public AbstractTemperatureSource
    {
      public:
        BellTemperatureSource()
        {
            bsp::bell_temp_sensor::init();
        }
        ~BellTemperatureSource()
        {
            bsp::bell_temp_sensor::deinit();
        }
        Result read()
        {
            return bsp::bell_temp_sensor::readout();
        }
    };

    std::shared_ptr<AbstractTemperatureSource> AbstractTemperatureSource::Factory::create()
    {
        return hal::impl::factory<BellTemperatureSource, AbstractTemperatureSource>();
    }
} // namespace hal::temperature

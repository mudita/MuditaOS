// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>
#include <hal/GenericFactory.hpp>

namespace hal::temperature
{

    class RT1051TemperatureSource : public AbstractTemperatureSource
    {
      public:
        Result read() override
        {
            return hal::temperature::AbstractTemperatureSource::Result();
        }
    };

    std::shared_ptr<AbstractTemperatureSource> AbstractTemperatureSource::Factory::create()
    {
        return hal::impl::factory<RT1051TemperatureSource, AbstractTemperatureSource>();
    }
} // namespace hal::temperature

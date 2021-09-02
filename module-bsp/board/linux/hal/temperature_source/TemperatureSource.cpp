// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>
#include <hal/GenericFactory.hpp>

namespace hal::temperature
{

    class LinuxTemperatureSource : public AbstractTemperatureSource
    {
      public:
        Result read()
        {
            temperature += 1.0;
            if (temperature > limitUp) {
                temperature = limitDown;
            }
            return temperature;
        }

      private:
        static constexpr AbstractTemperatureSource::Temperature limitUp   = 10.0;
        static constexpr AbstractTemperatureSource::Temperature limitDown = -10.0;
        AbstractTemperatureSource::Temperature temperature{};
    };

    std::shared_ptr<AbstractTemperatureSource> AbstractTemperatureSource::Factory::create()
    {
        return hal::impl::factory<LinuxTemperatureSource, AbstractTemperatureSource>();
    }
} // namespace hal::temperature

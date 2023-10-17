// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "hal/temperature_source/TemperatureSource.hpp"
#include "hal/GenericFactory.hpp"
#include "devices/temperature/CT7117.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"

#include <stdexcept>

namespace hal::temperature
{
    using namespace drivers;

    class PowerEnable
    {
      public:
        PowerEnable()
        {
            power_pin = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_TEMP_SENSOR_PWR_GPIO),
                                           DriverGPIOParams{});

            power_pin->ConfPin(
                DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                    .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                    .defLogic = 1,
                                    .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_TEMP_SENSOR_PWR_PIN)});

            power_pin->WritePin(static_cast<uint32_t>(BoardDefinitions::BELL_TEMP_SENSOR_PWR_PIN), 1);
        }
        ~PowerEnable()
        {
            power_pin->WritePin(static_cast<uint32_t>(BoardDefinitions::BELL_TEMP_SENSOR_PWR_PIN), 0);
        }

      private:
        std::shared_ptr<drivers::DriverGPIO> power_pin;
    };

    class BellTemperatureSource : public AbstractTemperatureSource
    {
      public:
        BellTemperatureSource()
            : i2c{drivers::DriverI2C::Create(
                  static_cast<drivers::I2CInstances>(BoardDefinitions::BELL_TEMP_SENSOR_I2C),
                  {static_cast<std::uint32_t>(BoardDefinitions::BELL_TEMP_SENSOR_I2C_BAUDRATE)})},
              ct7117{ct7117_id, *i2c}
        {
            if (not ct7117.poll()) {
                throw std::runtime_error("CT7117 chip not present");
            }
            if (not ct7117.wakeup()) {
                throw std::runtime_error("Waking up CT7117 failed");
            }
        }
        Result read()
        {
            return ct7117.get_temperature();
        }

      private:
        static constexpr auto ct7117_id = (0x90 >> 1);
        PowerEnable power_enable;
        std::shared_ptr<drivers::DriverI2C> i2c;
        bsp::devices::temperature::CT7117::CT7117 ct7117;
    };

    std::shared_ptr<AbstractTemperatureSource> AbstractTemperatureSource::Factory::create()
    {
        return hal::impl::factory<BellTemperatureSource, AbstractTemperatureSource>();
    }
} // namespace hal::temperature

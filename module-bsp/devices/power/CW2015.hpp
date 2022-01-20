// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "hal/Units.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "drivers/i2c/DriverI2C.hpp"

namespace bsp::devices::power
{
    /// MP2639B fuel gauge driver
    /// To enable interrupts support call \ref init_irq_pin and then invoke \ref handle_irq in the corresponding IRQ
    /// routine.
    class CW2015
    {
      public:
        explicit CW2015(std::shared_ptr<drivers::DriverI2C> i2c);
        ~CW2015();

        /// Returns the result of the chip initialization
        std::int8_t poll() const;

        std::optional<units::SOC> get_battery_soc();

        units::Voltage get_battery_voltage();

        void init_irq_pin(std::shared_ptr<drivers::DriverGPIO> gpio, uint32_t pin);
        void irq_handler();

      private:
        void clear_irq();

        std::int8_t init_chip();
        std::int8_t reset_chip();
        std::int8_t update_chip_config();

        std::int8_t quick_start();

        std::shared_ptr<drivers::DriverI2C> i2c;
        std::int8_t state{};
    };

} // namespace bsp::devices::power

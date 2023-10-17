// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/i2c/DriverI2C.hpp"
#include <Units.hpp>

#include <optional>

namespace bsp::devices::temperature::CT7117
{
    class CT7117
    {
      public:
        CT7117(std::uint8_t id, drivers::DriverI2C &i2c);
        ~CT7117();
        CT7117(const CT7117 &)     = delete;
        CT7117(CT7117 &&) noexcept = default;
        CT7117 &operator=(const CT7117 &) = delete;
        CT7117 &operator=(CT7117 &&) noexcept = delete;

        /// Check if the chip is available, i.e is present on the I2C bus
        bool poll() const;
        /// Put the chip into low-power state
        bool standby();
        /// Put the chip into normal/operational mode
        bool wakeup();
        /// Return the current temperature in °C
        std::optional<units::Temperature> get_temperature() const;

      private:
        std::uint8_t device_id;
        drivers::DriverI2C &i2c;
    };

} // namespace bsp::devices::temperature::CT7117

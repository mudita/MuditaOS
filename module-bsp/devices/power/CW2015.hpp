// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/gpio/DriverGPIO.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#include <Units.hpp>

namespace bsp::devices::power
{
    /// MP2639B fuel gauge driver
    /// To enable interrupts support call \ref init_irq_pin and then invoke \ref handle_irq in the corresponding IRQ
    /// routine.
    class CW2015
    {
      public:
        enum class RetCodes : std::uint8_t
        {
            Ok,
            CommunicationError,
            ProfileInvalid,
            NotReady
        };

        CW2015(drivers::DriverI2C &i2c, units::SOC soc);
        ~CW2015();

        CW2015(const CW2015 &)     = delete;
        CW2015(CW2015 &&) noexcept = default;
        CW2015 &operator=(const CW2015 &) = delete;
        CW2015 &operator=(CW2015 &&) noexcept = delete;

        /// Returns the result of the chip initialization
        RetCodes poll() const;
        /// Returns true if the chip initialization success, otherwise false
        explicit operator bool() const;
        /// Explicitly triggers chip initialization procedure. The result can be polled by \ref poll or using bool
        /// operator
        void reinit();

        RetCodes calibrate();

        std::optional<units::SOC> get_battery_soc();

        std::optional<units::Voltage> get_battery_voltage();

        void init_irq_pin(std::shared_ptr<drivers::DriverGPIO> gpio, uint32_t pin);
        void irq_handler();

      private:
        /// Might return valid value which then can be tested for true of false
        using TriState = std::optional<bool>;

        RetCodes clear_irq();

        RetCodes init_chip();
        RetCodes reset_chip();
        RetCodes load_profile();
        RetCodes verify_profile();
        RetCodes write_profile();

        RetCodes quick_start();
        RetCodes sleep();
        RetCodes wake_up();
        RetCodes set_soc_threshold(std::uint8_t threshold);
        RetCodes set_update_flag();
        RetCodes wait_for_ready();
        TriState is_update_needed();
        TriState is_sleep_mode();

        void dumpRegisters();

        std::optional<std::uint8_t> read(std::uint8_t reg);
        bool write(std::uint8_t reg, std::uint8_t value);

        drivers::DriverI2C &i2c;
        RetCodes status{};
        units::SOC alert_threshold;
    };

} // namespace bsp::devices::power

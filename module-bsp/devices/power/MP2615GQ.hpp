// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "timers.h"

#include <drivers/gpio/DriverGPIO.hpp>

#include <functional>

namespace bsp::devices::power
{
    /// MP2615GQ charger driver
    /// To enable interrupts support call \ref enable_irq and then invoke \ref handle_irq in the corresponding IRQ
    /// routine.
    class MP2615GQ
    {
      public:
        enum class ChargingStatus : std::uint8_t
        {
            Discharging,
            Charging,
            Complete,
            PluggedNotCharging, /// Charger plugged-in, input voltage valid but charging explicitly disabled
            Error,
        };

        struct Configuration
        {
            std::shared_ptr<drivers::DriverGPIO> mode_gpio;
            uint32_t mode_pin;

            std::shared_ptr<drivers::DriverGPIO> acok_gpio;
            uint32_t acok_pin;

            std::shared_ptr<drivers::DriverGPIO> chgok_gpio;
            uint32_t chgok_pin;

            /// User defined notification handler called when the driver wants to inform that charging state has
            /// changed. Notifications are called from the software timer context hence it is not allowed to perform
            /// blocking or memory demanding operations from it.
            std::function<void(const ChargingStatus)> notification;
        };

        explicit MP2615GQ(const Configuration &conf);
        ~MP2615GQ();

        MP2615GQ(const MP2615GQ &) = delete;
        MP2615GQ &operator=(const MP2615GQ &) = delete;
        MP2615GQ(MP2615GQ &&)                 = default;
        MP2615GQ &operator=(MP2615GQ &&) = delete;

        ChargingStatus get_charge_status();
        void enable_charging(bool ctrl);

        void enable_irq();
        void handle_irq();

      private:
        [[nodiscard]] bool is_charging() const;
        [[nodiscard]] bool is_valid_voltage() const;
        [[nodiscard]] bool is_charging_enabled() const;

        static constexpr auto irq_filter_timer_interval_ms    = 50;
        static constexpr auto irq_filter_timer_name           = "irq_charger_filter";
        static constexpr auto irq_filter_timer_cmd_timeout_ms = 100;

        Configuration configuration;
        xTimerHandle irq_filter_timer;
    };
} // namespace bsp::devices::power

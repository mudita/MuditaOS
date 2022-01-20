// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "timers.h"

#include <drivers/gpio/DriverGPIO.hpp>

#include <functional>

namespace bsp::devices::power
{
    /// MP2639B charger driver
    /// To enable interrupts support call \ref enable_irq and then invoke \ref handle_irq in the corresponding IRQ
    /// routine.
    class MP2639B
    {
      public:
        enum class ChargingStatus
        {
            Discharging,
            Charging,
            Complete,
            Error
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

        explicit MP2639B(const Configuration &conf);
        ~MP2639B();

        MP2639B(const MP2639B &) = delete;
        MP2639B &operator=(const MP2639B &) = delete;
        MP2639B(MP2639B &&)                 = default;
        MP2639B &operator=(MP2639B &&) = delete;

        ChargingStatus get_charge_status();
        void enable_charging(bool ctrl);

        void enable_irq();
        void handle_irq();

      private:
        Configuration configuration;
        xTimerHandle irq_filter_timer;
    };

} // namespace bsp::devices::power

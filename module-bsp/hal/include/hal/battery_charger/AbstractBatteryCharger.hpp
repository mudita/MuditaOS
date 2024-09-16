// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Units.hpp>
#include <FreeRTOS.h>
#include <queue.h>

#include <optional>
#include <memory>
#include <cstdint>

namespace hal::battery
{
    class AbstractBatteryCharger
    {
      public:
        using SOC     = units::SOC;
        using Voltage = units::Voltage;

        enum class ChargingStatus
        {
            Discharging,
            Charging,
            ChargingDone,
            PluggedNotCharging /// Charger plugged-in, input voltage valid but charging explicitly disabled
        };

        enum class Events : std::uint8_t
        {
            Charger,
            SOC,
            Brownout
        };

        enum class ChargerPresence : std::uint8_t
        {
            Undefined,
            PluggedIn,
            Unplugged
        };

        enum class TemperatureState : std::uint8_t
        {
            Normal,
            TooLow,
            TooHigh,
            Unknown
        };

        struct Factory
        {
            static std::unique_ptr<AbstractBatteryCharger> create(xQueueHandle irqQueueHandle);
        };

        virtual ~AbstractBatteryCharger() = default;

        [[nodiscard]] virtual std::optional<Voltage> getBatteryVoltage() const = 0;
        [[nodiscard]] virtual std::optional<SOC> getSOC() const                = 0;
        [[nodiscard]] virtual ChargingStatus getChargingStatus() const         = 0;
        [[nodiscard]] virtual ChargerPresence getChargerPresence() const       = 0;
        [[nodiscard]] virtual TemperatureState getTemperatureState() const     = 0;

        static_assert(sizeof(Events) == sizeof(std::uint8_t),
                      "All events processed by event manager ought to have size of std::uint8_t");
    };
} // namespace hal::battery

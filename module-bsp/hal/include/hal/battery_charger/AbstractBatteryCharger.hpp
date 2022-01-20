// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "hal/Units.hpp"

#include <FreeRTOS.h>
#include <queue.h>

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
            PluggedNotCharging
        };

        enum class Events : std::uint8_t
        {
            Charger,
            SOC,
            Brownout
        };

        struct Factory
        {
            static std::unique_ptr<AbstractBatteryCharger> create(xQueueHandle);
        };

        virtual ~AbstractBatteryCharger() = default;

        virtual Voltage getBatteryVoltage() const        = 0;
        virtual SOC getSOC() const                       = 0;
        virtual ChargingStatus getChargingStatus() const = 0;

        static_assert(sizeof(Events) == sizeof(std::uint8_t),
                      "All events processed by event manager ought to have size of std::uint8_t");
    };
} // namespace hal::battery

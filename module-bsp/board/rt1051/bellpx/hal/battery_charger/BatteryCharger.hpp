// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <bsp/fuel_gauge/fuel_gauge.hpp>

namespace hal::battery
{
    class BatteryCharger : public AbstractBatteryCharger
    {
      public:
        explicit BatteryCharger(AbstractBatteryCharger::BatteryChargerEvents &eventsHandler);
        void init(xQueueHandle queueBatteryHandle, xQueueHandle) final;
        void deinit() final;
        void processStateChangeNotification(std::uint8_t notification) final;
        void setChargingCurrentLimit(std::uint8_t) final;
        int getBatteryVoltage() final;

      private:
        bsp::fuel_gauge::StateOfCharge filterSOC();

        AbstractBatteryCharger::BatteryChargerEvents &eventsHandler;
        bsp::fuel_gauge::StateOfCharge lastSOC;
    };
} // namespace hal::battery

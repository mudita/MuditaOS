// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/hal/battery_charger/AbstractBatteryCharger.hpp>
#include <service-evtmgr/battery-brownout-detector/BatteryBrownoutDetector.hpp>

#include <memory>

namespace hal::battery
{
    class BatteryCharger : public AbstractBatteryCharger
    {
      public:
        explicit BatteryCharger(sys::Service *service);
        void init(xQueueHandle queueBatteryHandle, xQueueHandle queueChargerDetect) final;
        void deinit() final;
        void processStateChangeNotification(std::uint8_t notification) final;
        void setChargingCurrentLimit(std::uint8_t chargerType) final;

      private:
        void checkBatteryChargerInterrupts();

        sys::Service *service;
        BatteryBrownoutDetector batteryBrownoutDetector;
    };
} // namespace hal::battery

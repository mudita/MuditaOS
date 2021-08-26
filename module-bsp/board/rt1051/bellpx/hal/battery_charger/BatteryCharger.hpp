// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/hal/battery_charger/AbstractBatteryCharger.hpp>

#include <memory>

namespace hal::battery
{
    class BatteryCharger : public AbstractBatteryCharger
    {
      public:
        explicit BatteryCharger(sys::Service *);
        void init(xQueueHandle queueBatteryHandle, xQueueHandle) final;
        void deinit() final;
        void processStateChangeNotification(std::uint8_t notification) final;
        void setChargingCurrentLimit(std::uint8_t) final;

      private:
        sys::Service *service;
    };
} // namespace hal::battery

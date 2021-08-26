// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/hal/battery_charger/AbstractBatteryCharger.hpp>

#include <memory>

namespace hal::battery
{
    class LinuxBatteryCharger : public AbstractBatteryCharger
    {
      public:
        explicit LinuxBatteryCharger(sys::Service *service);
        void init(xQueueHandle irqQueueHandle, xQueueHandle dcdQueueHandle) final;
        void deinit() final;
        void processStateChangeNotification(std::uint8_t notification) final;
        void setChargingCurrentLimit(std::uint8_t usbType) final;

      private:
        sys::Service *service;
    };
} // namespace hal::battery

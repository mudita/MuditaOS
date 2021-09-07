// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <queue.h>
#include <Service/Service.hpp>

#include <memory>

namespace hal::battery
{
    class AbstractBatteryCharger
    {
      public:
        struct Factory
        {
            static std::shared_ptr<AbstractBatteryCharger> create(sys::Service *service);
        };

        virtual ~AbstractBatteryCharger() = default;

        virtual void init(xQueueHandle, xQueueHandle)             = 0;
        virtual void deinit()                                     = 0;
        virtual void processStateChangeNotification(std::uint8_t) = 0;
        virtual void setChargingCurrentLimit(std::uint8_t)        = 0;
    };

    BaseType_t IRQHandler();
    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t detectedType);
    }
} // namespace hal::battery

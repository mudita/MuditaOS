// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/battery_charger/BatteryCharger.hpp>
#include <battery-brownout-detector/BatteryBrownoutDetector.hpp>
#include <Service/Service.hpp>
#include <queue.hpp>

#include <memory>

#include <cstdint>

namespace sevm::battery
{
    class BatteryController : public hal::battery::AbstractBatteryCharger::BatteryChargerEvents,
                              public std::enable_shared_from_this<BatteryController>
    {
      public:
        explicit BatteryController(sys::Service *service);

        void onBrownout() final;
        void onStatusChanged() final;

        void init(xQueueHandle queueBatteryHandle, xQueueHandle queueChargerDetect);
        void deinit();
        void handleChargerNotification(std::uint8_t notification);
        void handleBatteryNotification(std::uint8_t notification);

      private:
        sys::Service *service = nullptr;
        std::shared_ptr<hal::battery::AbstractBatteryCharger> charger;
        BatteryBrownoutDetector brownoutDetector;
    };
}; // namespace sevm::battery

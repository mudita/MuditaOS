// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include "BatteryBrownoutDetector.hpp"
#include "BatteryState.hpp"

#include <queue.hpp>
#include <memory>
#include <cstdint>

namespace sevm::battery
{
    /// Battery controller class that glues various submodules together. It does not perform any logic by itself but
    /// just routes notifications and messages received from corresponding submodules.
    class BatteryController
    {
      public:
        using Events          = hal::battery::AbstractBatteryCharger::Events;
        using ChargerPresence = hal::battery::AbstractBatteryCharger::ChargerPresence;
        BatteryController(sys::Service *service, xQueueHandle notificationChannel, BatteryState::Thresholds thresholds);

        void poll();

        /// Handler for incoming async notifications from the back-end
        void handleNotification(Events);

      private:
        void update();
        void updateSoC();
        void printCurrentState();
        void checkChargerPresence();
        sys::Service *service{nullptr};
        std::unique_ptr<hal::battery::AbstractBatteryCharger> charger;
        BatteryBrownoutDetector brownoutDetector;
        BatteryState batteryState;
        ChargerPresence chargerPresence{ChargerPresence::Undefined};
    };
}; // namespace sevm::battery

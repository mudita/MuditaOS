// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <battery/BatteryController.hpp>

namespace sevm::battery::bell
{
    class BatteryController : public battery::BatteryController
    {
      public:
        using Events          = hal::battery::AbstractBatteryCharger::Events;
        using ChargerPresence = hal::battery::AbstractBatteryCharger::ChargerPresence;
        explicit BatteryController(sys::Service *service, xQueueHandle notificationChannel);

        void update() override;

      private:
        void sendLowVoltageShutdownMessage();
    };
}; // namespace sevm::battery::bell

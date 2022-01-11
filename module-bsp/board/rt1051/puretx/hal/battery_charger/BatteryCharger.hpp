// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <timers.h>

namespace hal::battery
{
    class BatteryCharger : public AbstractBatteryCharger
    {
      public:
        explicit BatteryCharger(AbstractBatteryCharger::BatteryChargerEvents &eventsHandler);
        void init(xQueueHandle queueBatteryHandle, xQueueHandle queueChargerDetect) final;
        void deinit() final;
        void processStateChangeNotification(std::uint8_t notification) final;
        void setChargingCurrentLimit(std::uint8_t chargerType) final;
        int getBatteryVoltage() final;

      private:
        void checkBatteryChargerInterrupts();

        AbstractBatteryCharger::BatteryChargerEvents &eventsHandler;
        TimerHandle_t timerHandle;
    };

    BaseType_t INTBHandlerIRQ();
    BaseType_t INOKBHandlerIRQ();
} // namespace hal::battery

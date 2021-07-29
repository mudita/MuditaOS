// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

namespace bsp::fuel_gauge
{
    using StateOfCharge = std::uint8_t;

    auto constexpr FuelGaugeAlert = 0x01;
    auto constexpr FuelGaugeUpdate = 0x02;
    auto constexpr FuelGaugeAlertCapacityPercent = 10;   //10%

    void init(xQueueHandle irqQueueHandle);

    void deinit();

    StateOfCharge getBatteryLevel();

    int getVoltageFilteredMeasurement();

    BaseType_t ALRT_IRQHandler();

} // bsp::fuel_gauge




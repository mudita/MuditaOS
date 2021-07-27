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

#include <array>

namespace bsp::battery_charger
{
    using StateOfCharge = std::uint8_t;

    enum class batteryRetval
    {
        ChargerError,
        ChargerNotCharging,
        ChargerCharging,
        ChargerComplete,
        ChargerStatus
    };

    int init(xQueueHandle irqQueueHandle);

    void deinit();

    bool getChargeStatus();

    void setChargerError();

    void enableCharging();

    void disableCharging();

    BaseType_t CHG_IRQHandler();

} // namespace bsp::battery_charger

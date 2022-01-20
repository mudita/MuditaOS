// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include "FreeRTOS.h"

namespace hal::battery
{
    BaseType_t charger_irq();
    BaseType_t fuel_gauge_irq();
} // namespace hal::battery

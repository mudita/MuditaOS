// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MIMXRT1052.h>

namespace board
{
    constexpr auto lowestIRQprio = (1U << __NVIC_PRIO_BITS) - 1;
    void irq_gpio_Init();
} // namespace board
